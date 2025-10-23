#define NOMINMAX
#include <SFML/Graphics.hpp>
#include <iostream>
#include <vector>
#include <string>
#include <queue>
#include <cmath>
#include <algorithm>
#include <chrono>
#include <cstdlib>
#include <cstdio>

#include "Dispatcher.h"
#include "Courier.h"
#include "Office.h"
#include "Letter.h"

struct Colors {
    sf::Color bg{ 25,27,34 };
    sf::Color panel{ 37,40,48 };
    sf::Color panelAlt{ 45,49,58 };
    sf::Color line{ 60,65,75 };
    sf::Color office{ 120,180,255 };
    sf::Color officeBorder{ 200,220,255 };
    sf::Color text{ 230,235,245 };
    sf::Color textDim{ 170,175,185 };
    sf::Color listRow{ 55,58,68 };
    sf::Color listRowAlt{ 50,53,63 };
    sf::Color hot{ 255,130,90 };
    sf::Color courierCol[6] = {
        sf::Color(255, 99, 132),
        sf::Color(54, 162, 235),
        sf::Color(255, 206, 86),
        sf::Color(75, 192, 192),
        sf::Color(153, 102, 255),
        sf::Color(255, 159, 64)
    };
} C;

static const unsigned WIN_W = 1280;
static const unsigned WIN_H = 820;

void drawText(sf::RenderTarget& rt, sf::Font* f, const std::string& s, float x, float y, unsigned size = 16, sf::Color col = C.text) {
    if (!f) return; sf::Text t; t.setFont(*f); t.setString(s); t.setCharacterSize(size); t.setFillColor(col); t.setPosition(x, y); rt.draw(t);
}
void drawPanel(sf::RenderTarget& rt, sf::FloatRect r, sf::Color fill) {
    sf::RectangleShape rs({ r.width, r.height }); rs.setPosition(r.left, r.top); rs.setFillColor(fill); rt.draw(rs);
}
void drawLine(sf::RenderTarget& rt, sf::Vector2f a, sf::Vector2f b, sf::Color col, float th = 2.f) {
    sf::Vector2f d = b - a; float len = std::sqrt(d.x * d.x + d.y * d.y); if (len <= 1e-3f) return;
    sf::RectangleShape ln({ len, th }); ln.setFillColor(col); ln.setPosition(a); ln.setRotation(std::atan2(d.y, d.x) * 180.f / 3.14159265f); rt.draw(ln);
}
struct MapTransform {
    sf::FloatRect world{ 0,0,30,30 };
    sf::FloatRect screen{ 30,30, 760, 760 };
    sf::Vector2f toScreen(const std::pair<int, int>& p) const {
        float u = (p.first - world.left) / world.width; float v = (p.second - world.top) / world.height;
        return { screen.left + u * screen.width, screen.top + v * screen.height };
    }
};
struct ScrollList {
    sf::FloatRect area; float headerH = 24.f, rowH = 22.f, scrollY = 0.f;
    void onWheel(float d, int rows) {
        float viewH = area.height - headerH; float maxScroll = std::max(0.f, rows * rowH - viewH);
        scrollY -= d * 40.f; if (scrollY < 0) scrollY = 0; if (scrollY > maxScroll) scrollY = maxScroll;
    }
};

static std::string fmtMin(int m) { return std::to_string(m) + " min"; }
static std::string fmtTimeHM(int m) { int H = 9 + m / 60, M = m % 60; char buf[16]; std::snprintf(buf, 16, "%02d:%02d", H, M); return buf; }

int main() {
    std::srand((unsigned)std::chrono::high_resolution_clock::now().time_since_epoch().count());

    int N = 5, M = 3, STEP = 30;
    Dispatcher disp(N, M, STEP);
    disp.programm_work();

    std::vector<std::pair<int, int>> offices; offices.reserve(N);
    for (int i = 0; i < N; ++i) offices.push_back(disp.get_ofice(i).get_pos());

    const std::vector<TripEvent>& events = disp.get_event_log();
    std::vector<std::vector<TripEvent>> dayEvents(7);
    for (const TripEvent& ev : events) {
        if (ev.day >= 0 && ev.day < 7) dayEvents[ev.day].push_back(ev);
    }
    for (size_t d = 0; d < dayEvents.size(); ++d) {
        std::sort(dayEvents[d].begin(), dayEvents[d].end(),
            [](const TripEvent& a, const TripEvent& b) {
                if (a.t_begin != b.t_begin) return a.t_begin < b.t_begin;
                if (a.courier_id != b.courier_id) return a.courier_id < b.courier_id;
                return a.t_end < b.t_end;
            });
    }
    std::cout << "[event counts per day]\n";
    for (int d = 0; d < 7; ++d) std::cout << "  day " << d << ": " << dayEvents[d].size() << " events\n";

    sf::RenderWindow win(sf::VideoMode(WIN_W, WIN_H), "Courier Service — Animated Week (SFML)");
    win.setFramerateLimit(60);

    sf::Font font; sf::Font* pf = nullptr;
    if (font.loadFromFile("font.ttf")) pf = &font; else std::cerr << "[warn] font.ttf missing.\n";

    MapTransform xf;

    const sf::FloatRect right{ 820, 30, 430, 760 };
    ScrollList listInc{ sf::FloatRect(right.left + 10, right.top + 110, right.width - 20, 240) };
    ScrollList listAct{ sf::FloatRect(right.left + 10, right.top + 410, right.width - 20, 180) };

    int   day = 0;
    float t = 0.f;
    float simSpeed = 1.f;
    bool  playing = true;

    struct CurState {
        sf::Vector2f pos; bool moving = false; int from = -1, to = -1; int t0 = 0, t1 = 0; bool deadhead = false; int letter_id = -1;
    };
    std::vector<CurState> cur(M);

    auto resetDay = [&]() {
        int guard = 0;
        while (day < 7 && dayEvents[day].empty() && guard++ < 8) day++;
        if (day > 6) day = 6;

        t = 0.f;
        listInc.scrollY = 0.f; listAct.scrollY = 0.f;

        for (int i = 0; i < M; ++i) {
            int startOffice = 0;
            if (!dayEvents[day].empty()) {
                for (const TripEvent& e : dayEvents[day]) { if (e.courier_id == i) { startOffice = e.from_of; break; } }
            }
            cur[i].pos = xf.toScreen(offices[startOffice]);
            cur[i].moving = false; cur[i].from = startOffice; cur[i].to = startOffice;
            cur[i].letter_id = -1; cur[i].deadhead = false; cur[i].t0 = 0; cur[i].t1 = 0;
        }
        };
    resetDay();

    auto updateCouriers = [&]() {
        for (int i = 0; i < M; ++i) {
            const auto& evs = dayEvents[day];
            const TripEvent* active = nullptr; const TripEvent* lastPast = nullptr;

            for (const TripEvent& e : evs) {
                if (e.courier_id != i) continue;
                if (e.t_begin <= (int)t && (int)t <= e.t_end) { active = &e; break; }
                if (e.t_end <= (int)t) lastPast = &e;
                if (e.t_begin > (int)t) break;
            }

            if (active) {
                auto A = xf.toScreen(offices[active->from_of]);
                auto B = xf.toScreen(offices[active->to_of]);
                float dur = std::max(1, active->t_end - active->t_begin);
                float a = std::clamp((t - active->t_begin) / dur, 0.f, 1.f);
                cur[i].pos = { A.x + (B.x - A.x) * a, A.y + (B.y - A.y) * a };
                cur[i].moving = true; cur[i].from = active->from_of; cur[i].to = active->to_of;
                cur[i].t0 = active->t_begin; cur[i].t1 = active->t_end; cur[i].deadhead = active->deadhead; cur[i].letter_id = active->letter_id;
            }
            else {
                int office = (lastPast ? lastPast->to_of : cur[i].from);
                cur[i].pos = xf.toScreen(offices[office]);
                cur[i].moving = false; cur[i].from = office; cur[i].to = office;
                cur[i].letter_id = -1; cur[i].deadhead = false; cur[i].t0 = 0; cur[i].t1 = 0;
            }
        }
        };

    sf::Clock clk;
    while (win.isOpen()) {
        sf::Event e;
        while (win.pollEvent(e)) {
            if (e.type == sf::Event::Closed) win.close();
            if (e.type == sf::Event::KeyPressed) {
                if (e.key.code == sf::Keyboard::Space) playing = !playing;
                if (e.key.code == sf::Keyboard::Num1)  simSpeed = 1.f;
                if (e.key.code == sf::Keyboard::Num2)  simSpeed = 2.f;
                if (e.key.code == sf::Keyboard::Num3)  simSpeed = 4.f;
                if (e.key.code == sf::Keyboard::Home)  t = 0.f;
                if (e.key.code == sf::Keyboard::End)   t = 540.f;

                if (e.key.code == sf::Keyboard::Left) {
                    if (day > 0) day--;
                    int guard = 0;
                    while (day >= 0 && dayEvents[day].empty() && guard++ < 8) day--;
                    if (day < 0) day = 0;
                    resetDay();
                }
                if (e.key.code == sf::Keyboard::Right) {
                    if (day < 6) day++;
                    int guard = 0;
                    while (day < 7 && dayEvents[day].empty() && guard++ < 8) day++;
                    if (day > 6) day = 6;
                    resetDay();
                }
            }
            if (e.type == sf::Event::MouseWheelScrolled) {
                auto mp = win.mapPixelToCoords({ (int)e.mouseWheelScroll.x,(int)e.mouseWheelScroll.y });
                float wh = e.mouseWheelScroll.delta;
                if (listInc.area.contains(mp)) listInc.onWheel(wh, (int)dayEvents[day].size());
                else if (listAct.area.contains(mp)) listAct.onWheel(wh, M);
            }
        }

        float dt = clk.restart().asSeconds();
        if (playing) {
            t += dt * simSpeed * 10.f;
            while (t >= 540.f - 1e-6f) {
                if (day < 6) {
                    day++;
                    int guard = 0; while (day < 7 && dayEvents[day].empty() && guard++ < 8) day++;
                    if (day > 6) { day = 6; t = 540.f; playing = false; break; }
                    resetDay();
                }
                else {
                    t = 540.f; playing = false; break;
                }
            }
        }
        updateCouriers();

        win.clear(C.bg);

        drawPanel(win, { 20,20, 780, 780 }, C.panel);
        for (int i = 0; i < N; ++i) for (int j = i + 1; j < N; ++j)
            drawLine(win, xf.toScreen(offices[i]), xf.toScreen(offices[j]), C.line, 1.f);
        for (int i = 0; i < N; ++i) {
            auto p = xf.toScreen(offices[i]); float r = 10.f;
            sf::CircleShape c(r); c.setPosition(p.x - r, p.y - r); c.setFillColor(C.office);
            c.setOutlineColor(C.officeBorder); c.setOutlineThickness(2.f); win.draw(c);
            drawText(win, pf, "O" + std::to_string(i), p.x + 12, p.y - 8, 14, C.text);
        }

        for (int i = 0; i < M; ++i) {
            if (cur[i].moving) {
                drawLine(win, xf.toScreen(offices[cur[i].from]), xf.toScreen(offices[cur[i].to]),
                    cur[i].deadhead ? sf::Color(180, 180, 180) : C.courierCol[i % 6], 3.f);
            }
        }
        for (int i = 0; i < M; ++i) {
            float rr = 8.f; sf::CircleShape cc(rr);
            cc.setPosition(cur[i].pos.x - rr, cur[i].pos.y - rr);
            cc.setFillColor(C.courierCol[i % 6]);
            cc.setOutlineColor(sf::Color::Black); cc.setOutlineThickness(1.5f); win.draw(cc);
            drawText(win, pf, "C" + std::to_string(i), cur[i].pos.x - 6, cur[i].pos.y - 24, 12, C.textDim);
        }

        drawPanel(win, { 820,30,430,760 }, C.panel);
        drawText(win, pf, "Day: " + std::to_string(day + 1) + "/7  Time: " + fmtTimeHM((int)t),
            830, 38, 18);
        drawText(win, pf, "[Space] Play/Pause   [1/2/3] Speed   [Left/Right] Day   [Home/End] Jump",
            830, 64, 14, C.textDim);

        {
            sf::FloatRect bar{ 830, 84, 410, 18 };
            drawPanel(win, bar, C.panelAlt);
            float u = std::clamp(t / 540.f, 0.f, 1.f);
            sf::RectangleShape prog({ bar.width * u, bar.height }); prog.setPosition(bar.left, bar.top);
            prog.setFillColor(C.hot); win.draw(prog);
            for (int m = 0; m <= 540; m += 60) {
                float x = bar.left + bar.width * (m / 540.f);
                drawLine(win, { x,bar.top + bar.height }, { x,bar.top + bar.height + 6 }, C.textDim, 2.f);
                drawText(win, pf, fmtTimeHM(m), x - 18, bar.top + bar.height + 8, 12, C.textDim);
            }
        }

        {
            sf::FloatRect area{ 830, 130, 410, 240 };
            drawPanel(win, area, C.panelAlt);
            sf::RectangleShape hdr({ area.width, 24 }); hdr.setPosition(area.left, area.top);
            hdr.setFillColor(sf::Color(50, 54, 64)); win.draw(hdr);
            drawText(win, pf, "Start  End   C   From->To   Type   LetterId", area.left + 6, area.top + 3, 14, C.textDim);
            drawLine(win, { area.left, area.top + 24 }, { area.left + area.width, area.top + 24 }, C.line, 1.f);

            float contentTop = area.top + 24;
            float viewH = area.height - 24;
            const auto& vec = dayEvents[day];

            float maxScroll = std::max(0.f, (float)vec.size() * listInc.rowH - viewH);
            if (listInc.scrollY > maxScroll) listInc.scrollY = maxScroll;

            for (size_t i = 0; i < vec.size(); ++i) {
                float top = contentTop + i * listInc.rowH - listInc.scrollY;
                if (top + listInc.rowH < contentTop) continue;
                if (top > contentTop + viewH) break;

                const TripEvent& ev = vec[i];
                sf::RectangleShape row({ area.width, listInc.rowH });
                row.setPosition(area.left, top);
                row.setFillColor((i % 2 == 0) ? C.listRow : C.listRowAlt); win.draw(row);

                char buf[256];
                std::snprintf(buf, sizeof(buf), "%5s  %5s  %2d   O%d->O%d   %s   %d",
                    fmtTimeHM(ev.t_begin).c_str(), fmtTimeHM(ev.t_end).c_str(),
                    ev.courier_id, ev.from_of, ev.to_of,
                    ev.deadhead ? "deadhead" : "delivery",
                    ev.letter_id);
                drawText(win, pf, buf, area.left + 6, top + 3, 14);
            }
            listInc.area = area; 
        }

        {
            sf::FloatRect area{ 830, 400, 410, 180 };
            drawPanel(win, area, C.panelAlt);
            sf::RectangleShape hdr({ area.width, 24 }); hdr.setPosition(area.left, area.top);
            hdr.setFillColor(sf::Color(50, 54, 64)); win.draw(hdr);
            drawText(win, pf, "Courier   Status", area.left + 6, area.top + 3, 14, C.textDim);
            drawLine(win, { area.left, area.top + 24 }, { area.left + area.width, area.top + 24 }, C.line, 1.f);

            float contentTop = area.top + 24;
            float viewH = area.height - 24;

            float maxScroll = std::max(0.f, (float)M * listAct.rowH - viewH);
            if (listAct.scrollY > maxScroll) listAct.scrollY = maxScroll;

            for (int i = 0; i < M; ++i) {
                float top = contentTop + i * listAct.rowH - listAct.scrollY;
                if (top + listAct.rowH < contentTop) continue;
                if (top > contentTop + viewH) break;

                sf::RectangleShape row({ area.width, listAct.rowH });
                row.setPosition(area.left, top);
                row.setFillColor((i % 2 == 0) ? C.listRow : C.listRowAlt); win.draw(row);

                std::string st;
                if (cur[i].moving) {
                    st = std::string(cur[i].deadhead ? "deadhead " : "delivery ");
                    st += "O" + std::to_string(cur[i].from) + "->O" + std::to_string(cur[i].to);
                    st += "  [" + fmtTimeHM(cur[i].t0) + "-" + fmtTimeHM(cur[i].t1) + "]";
                }
                else {
                    st = "idle at O" + std::to_string(cur[i].from);
                }
                drawText(win, pf, "C" + std::to_string(i), area.left + 6, top + 3, 14);
                drawText(win, pf, st, area.left + 80, top + 3, 14, cur[i].moving ? C.text : C.textDim);
            }
            listAct.area = area;
        }

        {
            sf::FloatRect area{ 830, 600, 410, 190 };
            drawPanel(win, area, C.panelAlt);
            drawText(win, pf, "Week stats (from backend)", area.left + 8, area.top + 8, 16);
            drawText(win, pf, "Free rides: " + std::to_string(disp.get_num_free_rides()), area.left + 8, area.top + 34, 14);
            drawText(win, pf, "Free ride time: " + fmtMin(disp.get_free_rides_time()), area.left + 8, area.top + 56, 14);
            drawText(win, pf, "Total lateness: " + fmtMin(disp.get_sum_of_deviations()), area.left + 8, area.top + 78, 14);
        }

        win.display();
    }

    return 0;
}
