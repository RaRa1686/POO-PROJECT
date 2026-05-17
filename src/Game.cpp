#include "Game.h"
#include "GameException.h"
#include <cmath>
#include <algorithm>
#include <iostream>
#include <sstream>

Game* Game::instance = nullptr;

namespace GC {
    const float W         = 960.f;
    const float H         = 600.f;
    const float STAND_B   = 280.f;   // tribunele se termina la 280
    const float GROUND_Y  = 540.f;
    const float GAME_TIME = 90.f;
    const float GOAL_W    = 90.f;
    const float GOAL_H    = 160.f;
    const float POST_W    = 10.f;
}

Game* Game::getInstance(){
    if(!instance) instance = new Game();
    return instance;
}

Game::Game()
    : window(sf::VideoMode({(unsigned)GC::W,(unsigned)GC::H}), "Head Football"),
      groundY(GC::GROUND_Y), timeLeft(GC::GAME_TIME),
      gameOver(false), vsAI(false), state(GameState::MENU)
{
    window.setFramerateLimit(60);

    if(!font.openFromFile("/System/Library/Fonts/Supplemental/Arial.ttf"))
        if(!font.openFromFile("/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf"))
            std::cout<<"Font negasit!\n";

    timerText.emplace(font,"90",26);
    timerText->setFillColor(sf::Color::White);
    timerText->setOutlineColor(sf::Color::Black);
    timerText->setOutlineThickness(2.f);
    timerText->setPosition({GC::W/2.f-14.f, 26.f});

    addObserver(this);
}

Game::~Game(){ instance=nullptr; }
void Game::addObserver(IObserver* o){ observers.push_back(o); }
void Game::notifyObservers(const std::string& e){ for(auto* o:observers) o->onEvent(e); }
void Game::onEvent(const std::string& ev){
    eventLog.push_back(ev);
    if(eventLog.size()>20) eventLog.pop_front();
    std::cout<<"[EVENT] "<<ev<<"\n";
}

void Game::demonstratePolymorphism(){
    Player* p=player2.get();
    std::cout<<"[POLY] Upcast - "<<p->getInfo()<<"\n";
    if(AIPlayer* ai=dynamic_cast<AIPlayer*>(p))
        std::cout<<"[POLY] Downcast - AI diff: "<<ai->getDifficulty()<<"\n";
    else std::cout<<"[POLY] player2=HumanPlayer\n";
}

void Game::initGame(bool ai){
    vsAI=ai; timeLeft=GC::GAME_TIME; gameOver=false;
    ball=std::make_unique<Ball>(GC::W/2.f,400.f,GC::GROUND_Y,GC::W);
    { std::istringstream iss("480 380"); iss>>*ball; }

    player1=std::make_unique<HumanPlayer>(
        220.f,GC::GROUND_Y,sf::Color(220,60,60),"Jucator 1",
        GC::GROUND_Y,GC::W,
        sf::Keyboard::Key::A,sf::Keyboard::Key::D,sf::Keyboard::Key::W);

    if(vsAI){
        auto* ap=new AIPlayer(740.f,GC::GROUND_Y,sf::Color(60,100,220),"Calculator",
                              GC::GROUND_Y,GC::W,2);
        ap->setBall(ball.get());
        player2=std::unique_ptr<Player>(ap);
    } else {
        player2=std::make_unique<HumanPlayer>(
            740.f,GC::GROUND_Y,sf::Color(60,100,220),"Jucator 2",
            GC::GROUND_Y,GC::W,
            sf::Keyboard::Key::Left,sf::Keyboard::Key::Right,sf::Keyboard::Key::Up);
    }
    demonstratePolymorphism();
    state=GameState::PLAYING;
    notifyObservers("Joc inceput! "+std::string(vsAI?"vs AI":"2 Jucatori"));
}

void Game::run(){
    sf::Clock clock;
    while(window.isOpen()){
        float dt=clock.restart().asSeconds();
        if(dt>0.05f) dt=0.05f;
        handleEvents(); update(dt); draw();
    }
}

void Game::handleEvents(){
    while(const std::optional ev=window.pollEvent()){
        if(ev->is<sf::Event::Closed>()) window.close();
        if(const auto* k=ev->getIf<sf::Event::KeyPressed>()){
            if(state==GameState::MENU){
                if(k->code==sf::Keyboard::Key::Num1) initGame(false);
                if(k->code==sf::Keyboard::Key::Num2) initGame(true);
                if(k->code==sf::Keyboard::Key::Escape) window.close();
            } else if(state==GameState::GAMEOVER){
                if(k->code==sf::Keyboard::Key::R){
                    player1->reset(); player2->reset();
                    ball->reset(); Ball::resetStats();
                    timeLeft=GC::GAME_TIME; state=GameState::PLAYING;
                }
                if(k->code==sf::Keyboard::Key::M) state=GameState::MENU;
            }
        }
    }
}

void Game::update(float dt){
    if(state!=GameState::PLAYING) return;
    timeLeft-=dt;
    if(timeLeft<=0.f){
        timeLeft=0.f; state=GameState::GAMEOVER;
        scoreBoard.clear();
        scoreBoard.addEntry(player1->getName(),player1->getScore());
        scoreBoard.addEntry(player2->getName(),player2->getScore());
        scoreBoard.sortDescending(); scoreBoard.print(std::cout);
        statsBoard.clear();
        statsBoard.addEntry("Viteza max",Ball::getMaxSpeed());
        statsBoard.print(std::cout);
        std::string w=player1->getScore()>player2->getScore()?player1->getName():
                      player2->getScore()>player1->getScore()?player2->getName():"Egalitate";
        notifyObservers("Joc terminat! Castigator: "+w);
        return;
    }
    player1->handleInput(); player2->handleInput();
    player1->update(dt); player2->update(dt);
    ball->update(dt);
    try{
        checkBallPlayerCollision(*player1);
        checkBallPlayerCollision(*player2);
        checkPlayerPlayerCollision();
        checkGoal();
    } catch(const GoalException& e){
        std::cout<<"[GOL] "<<e.what()<<"\n";
        notifyObservers(std::string("Gol! ")+e.what());
    } catch(const GameException& e){
        std::cout<<"[ERR] "<<e.what()<<"\n";
    }
    timerText->setString(std::to_string((int)timeLeft));
}

void Game::checkBallPlayerCollision(Player& player){
    sf::Vector2f bp=ball->getPosition(),hp=player.getHead().getPosition();
    float dx=bp.x-hp.x,dy=bp.y-hp.y,dist=std::sqrt(dx*dx+dy*dy);
    float minD=ball->getRadius()+player.getHead().getRadius();
    if(dist<minD&&dist>0.f){
        float nx=dx/dist,ny=dy/dist;
        ball->setPosition(ball->getPosition()+sf::Vector2f(nx*(minD-dist),ny*(minD-dist)));
        float spd=std::max(std::sqrt(ball->getVelocity().x*ball->getVelocity().x+
                                      ball->getVelocity().y*ball->getVelocity().y),400.f);
        try{ ball->applyImpulse(nx,ny,spd); }
        catch(const InvalidMoveException& e){
            const GameException& b=e; std::cout<<"[WARN] "<<b.what()<<"\n";
        }
    }
}

void Game::checkPlayerPlayerCollision(){
    sf::Vector2f p1=player1->getPosition(),p2=player2->getPosition();
    float dx=p2.x-p1.x,dy=p2.y-p1.y,dist=std::sqrt(dx*dx+dy*dy);
    const float minD=82.f;
    if(dist<minD&&dist>0.f){
        float ov=(minD-dist)/2.f,nx=dx/dist;
        sf::Vector2f n1=p1-sf::Vector2f(nx*ov,0.f),n2=p2+sf::Vector2f(nx*ov,0.f);
        n1.x=std::max(50.f,std::min(n1.x,GC::W-50.f));
        n2.x=std::max(50.f,std::min(n2.x,GC::W-50.f));
        player1->setPosition(n1); player2->setPosition(n2);
        sf::Vector2f v1=player1->getVelocity(),v2=player2->getVelocity();
        player1->setVelocity({-std::abs(nx)*80.f,v1.y});
        player2->setVelocity({ std::abs(nx)*80.f,v2.y});
    }
}

void Game::checkGoal(){
    sf::Vector2f pos=ball->getPosition(); float r=ball->getRadius();
    if(pos.x-r<GC::GOAL_W&&pos.y+r>GC::GROUND_Y-GC::GOAL_H){
        player2->addScore(); ball->reset();
        throw GoalException(2,"Gol in poarta stanga!");
    }
    if(pos.x+r>GC::W-GC::GOAL_W&&pos.y+r>GC::GROUND_Y-GC::GOAL_H){
        player1->addScore(); ball->reset();
        throw GoalException(1,"Gol in poarta dreapta!");
    }
}

// ══════════════════════════════════════════════════════════════
//  FUNDAL STADION INTUNECAT cu lumini galbene
// ══════════════════════════════════════════════════════════════
void Game::drawDarkStadium(){
    // Fundal gri-bleumarin
    sf::RectangleShape bg({GC::W,GC::H});
    bg.setFillColor(sf::Color(38,42,55));
    window.draw(bg);

    // Tribuna fundal mai deschisa
    sf::RectangleShape stand({GC::W,GC::STAND_B});
    stand.setPosition({0.f,0.f});
    stand.setFillColor(sf::Color(48,52,68));
    window.draw(stand);

    // Lumini stadion difuze (galbene/oranj)
    auto bigLight=[&](float lx, float ly, float r){
        sf::CircleShape halo3(r*1.5f);
        halo3.setOrigin({r*1.5f,r*1.5f});
        halo3.setPosition({lx,ly});
        halo3.setFillColor(sf::Color(255,220,150,25));
        window.draw(halo3);
        sf::CircleShape halo2(r*1.0f);
        halo2.setOrigin({r,r});
        halo2.setPosition({lx,ly});
        halo2.setFillColor(sf::Color(255,210,130,55));
        window.draw(halo2);
        sf::CircleShape halo1(r*0.5f);
        halo1.setOrigin({r*0.5f,r*0.5f});
        halo1.setPosition({lx,ly});
        halo1.setFillColor(sf::Color(255,230,160,160));
        window.draw(halo1);
        sf::CircleShape center(r*0.2f);
        center.setOrigin({r*0.2f,r*0.2f});
        center.setPosition({lx,ly});
        center.setFillColor(sf::Color(255,255,230,255));
        window.draw(center);
    };
    // Rand sus de lumini
    for(int i=0;i<8;i++){
        float x = 80.f + i*115.f;
        bigLight(x, 65.f, 28.f);
    }
    // Lumini mai mici in rand 2
    for(int i=0;i<10;i++){
        float x = 30.f + i*100.f;
        bigLight(x, 145.f, 18.f);
    }

    // Public dens (capete inchise la culoare)
    std::vector<sf::Color> crowdDark={
        sf::Color(40,45,60), sf::Color(50,55,70),
        sf::Color(60,50,55), sf::Color(45,55,65),
        sf::Color(70,60,55), sf::Color(55,60,75),
    };
    for(int row=0;row<3;row++){
        for(int col=0;col<70;col++){
            float px = 5.f + col*14.f;
            float py = 195.f + row*28.f;
            int ci = (row*5+col*3) % (int)crowdDark.size();
            sf::CircleShape head(5.f);
            head.setOrigin({5.f,5.f});
            head.setPosition({px,py});
            head.setFillColor(crowdDark[ci]);
            window.draw(head);
            sf::RectangleShape body({10.f,16.f});
            body.setPosition({px-5.f,py+3.f});
            body.setFillColor(sf::Color(crowdDark[ci].r+20,crowdDark[ci].g+20,crowdDark[ci].b+20));
            window.draw(body);
        }
    }

    // Banda LED publicitate
    sf::RectangleShape led({GC::W,16.f});
    led.setPosition({0.f,GC::STAND_B-22.f});
    led.setFillColor(sf::Color(100,90,140));
    window.draw(led);
    std::vector<sf::Color> ledColors={
        sf::Color(220,80,80),sf::Color(80,200,80),sf::Color(80,120,220),
        sf::Color(240,180,60),sf::Color(200,80,200)
    };
    for(int i=0;i<24;i++){
        sf::RectangleShape patch({40.f,16.f});
        patch.setPosition({i*40.f,GC::STAND_B-22.f});
        patch.setFillColor(sf::Color(ledColors[i%5].r,ledColors[i%5].g,ledColors[i%5].b,150));
        window.draw(patch);
    }

    // Umbra spre teren
    for(int i=0;i<20;i++){
        sf::RectangleShape sh({GC::W,3.f});
        sh.setPosition({0.f,GC::STAND_B-6.f+i*0.3f});
        sh.setFillColor(sf::Color(0,0,0, (uint8_t)(60-i*3)));
        window.draw(sh);
    }

    // TEREN DREPTUNGHIULAR VERDE
    float pitchH = GC::H-GC::STAND_B;
    int stripes=12;
    float sw=GC::W/stripes;
    for(int i=0;i<stripes;i++){
        sf::RectangleShape s({sw+1.f,pitchH});
        s.setPosition({i*sw,GC::STAND_B});
        s.setFillColor(i%2==0?sf::Color(55,170,55):sf::Color(45,150,45));
        window.draw(s);
    }

    // Gazon sub linia de ground
    sf::RectangleShape bot({GC::W,GC::H-GC::GROUND_Y+2.f});
    bot.setPosition({0.f,GC::GROUND_Y});
    bot.setFillColor(sf::Color(55,170,55));
    window.draw(bot);

    // Linia de ground
    sf::RectangleShape gl({GC::W,4.f});
    gl.setPosition({0.f,GC::GROUND_Y});
    gl.setFillColor(sf::Color::White);
    window.draw(gl);

    // Marcaje albe
    sf::Color lineC(255,255,255,210);

    // Linia de centru
    sf::RectangleShape midV({4.f,GC::GROUND_Y-GC::STAND_B});
    midV.setPosition({GC::W/2.f-2.f,GC::STAND_B});
    midV.setFillColor(lineC); window.draw(midV);

    // Cerc de centru
    float cr=80.f;
    float cy=GC::STAND_B+(GC::GROUND_Y-GC::STAND_B)*0.5f;
    sf::CircleShape circ(cr);
    circ.setOrigin({cr,cr}); circ.setPosition({GC::W/2.f,cy});
    circ.setFillColor(sf::Color::Transparent);
    circ.setOutlineColor(lineC); circ.setOutlineThickness(4.f);
    window.draw(circ);

    sf::CircleShape dot(5.f);
    dot.setOrigin({5.f,5.f}); dot.setPosition({GC::W/2.f,cy});
    dot.setFillColor(lineC); window.draw(dot);

    // Suprafete de poarta
    float bW=GC::GOAL_W+30.f, bH=85.f;
    sf::RectangleShape bL({bW,bH});
    bL.setPosition({GC::GOAL_W,GC::GROUND_Y-bH});
    bL.setFillColor(sf::Color::Transparent);
    bL.setOutlineColor(lineC); bL.setOutlineThickness(3.5f);
    window.draw(bL);

    sf::RectangleShape bR({bW,bH});
    bR.setPosition({GC::W-GC::GOAL_W-bW,GC::GROUND_Y-bH});
    bR.setFillColor(sf::Color::Transparent);
    bR.setOutlineColor(lineC); bR.setOutlineThickness(3.5f);
    window.draw(bR);
}

// ══════════════════════════════════════════════════════════════
//  POARTA
// ══════════════════════════════════════════════════════════════
void Game::drawGoal(bool leftGoal){
    const float GW=GC::GOAL_W, GH=GC::GOAL_H;
    const float GY=GC::GROUND_Y, PW=GC::POST_W;
    float left=leftGoal?0.f:GC::W-GW;

    // Plasa
    sf::RectangleShape net({GW,GH});
    net.setPosition({left,GY-GH});
    net.setFillColor(sf::Color(245,245,245,200));
    window.draw(net);

    // Linii orizontale
    for(int i=0;i<=12;i++){
        sf::RectangleShape l({GW,1.f});
        l.setPosition({left,GY-GH+i*GH/12.f});
        l.setFillColor(sf::Color(160,160,160,220));
        window.draw(l);
    }
    // Linii verticale
    for(int i=0;i<=6;i++){
        sf::RectangleShape l({1.f,GH});
        l.setPosition({left+i*GW/6.f,GY-GH});
        l.setFillColor(sf::Color(160,160,160,220));
        window.draw(l);
    }

    // Stalpi
    sf::Color postC(255,255,255);
    sf::Color postOut(180,180,180);

    float outerX=leftGoal?left:left+GW-PW;
    sf::RectangleShape outerPost({PW,GH+PW});
    outerPost.setPosition({outerX,GY-GH});
    outerPost.setFillColor(postC);
    outerPost.setOutlineColor(postOut);
    outerPost.setOutlineThickness(2.f);
    window.draw(outerPost);

    float innerX=leftGoal?left+GW-PW:left;
    sf::RectangleShape innerPost({PW,GH+PW});
    innerPost.setPosition({innerX,GY-GH});
    innerPost.setFillColor(postC);
    innerPost.setOutlineColor(postOut);
    innerPost.setOutlineThickness(2.f);
    window.draw(innerPost);

    // Bara de sus
    sf::RectangleShape bar({GW,PW});
    bar.setPosition({left,GY-GH});
    bar.setFillColor(postC);
    bar.setOutlineColor(postOut);
    bar.setOutlineThickness(2.f);
    window.draw(bar);
}

// ══════════════════════════════════════════════════════════════
//  TABELA
// ══════════════════════════════════════════════════════════════
void Game::drawScoreboard(){
    float cx=GC::W/2.f;

    sf::RectangleShape board({280.f,72.f});
    board.setOrigin({140.f,0.f});
    board.setPosition({cx,8.f});
    board.setFillColor(sf::Color(35,35,40,240));
    board.setOutlineColor(sf::Color(100,90,60));
    board.setOutlineThickness(2.f);
    window.draw(board);

    sf::RectangleShape timerBox({70.f,72.f});
    timerBox.setOrigin({35.f,0.f});
    timerBox.setPosition({cx,8.f});
    timerBox.setFillColor(sf::Color(80,15,15,240));
    window.draw(timerBox);

    // Minge deasupra
    sf::CircleShape bIcon(13.f);
    bIcon.setOrigin({13.f,13.f});
    bIcon.setPosition({cx,8.f});
    bIcon.setFillColor(sf::Color::White);
    bIcon.setOutlineColor(sf::Color(60,60,60));
    bIcon.setOutlineThickness(2.f);
    window.draw(bIcon);
    sf::CircleShape spot(4.f,5);
    spot.setOrigin({4.f,4.f});
    spot.setPosition({cx,8.f});
    spot.setFillColor(sf::Color(40,40,40));
    window.draw(spot);

    std::string lbl1 = vsAI?"PLAYER":"P1";
    sf::Text t1(font,lbl1,14);
    t1.setFillColor(sf::Color(255,200,80));
    t1.setPosition({cx-130.f,12.f});
    window.draw(t1);

    std::string lbl2 = vsAI?"CPU":"P2";
    sf::Text t2(font,lbl2,14);
    t2.setFillColor(sf::Color(255,200,80));
    t2.setPosition({cx+92.f,12.f});
    window.draw(t2);

    sf::Text s1(font,std::to_string(player1->getScore()),40);
    s1.setFillColor(sf::Color::White);
    s1.setOutlineColor(sf::Color::Black);
    s1.setOutlineThickness(2.f);
    s1.setPosition({cx-120.f,26.f});
    window.draw(s1);

    window.draw(*timerText);

    sf::Text s2(font,std::to_string(player2->getScore()),40);
    s2.setFillColor(sf::Color::White);
    s2.setOutlineColor(sf::Color::Black);
    s2.setOutlineThickness(2.f);
    s2.setPosition({cx+80.f,26.f});
    window.draw(s2);
}

// ══════════════════════════════════════════════════════════════
//  DRAW
// ══════════════════════════════════════════════════════════════
void Game::draw(){
    window.clear();
    if(state==GameState::MENU){ drawMenu(); window.display(); return; }
    drawGame();
    if(state==GameState::GAMEOVER) drawGameOver();
    window.display();
}

void Game::drawGame(){
    drawDarkStadium();
    drawGoal(true); drawGoal(false);
    player1->draw(window);
    player2->draw(window);
    window.draw(ball->getShape());
    drawScoreboard();
}

void Game::drawMenu(){
    drawDarkStadium();
    drawGoal(true); drawGoal(false);

    sf::RectangleShape ov({GC::W,GC::H});
    ov.setFillColor(sf::Color(0,0,10,140));
    window.draw(ov);

    float cx=GC::W/2.f, cy=110.f;

    sf::CircleShape outerRing(52.f);
    outerRing.setOrigin({52.f,52.f}); outerRing.setPosition({cx,cy});
    outerRing.setFillColor(sf::Color::Transparent);
    outerRing.setOutlineColor(sf::Color(200,165,70));
    outerRing.setOutlineThickness(3.f); window.draw(outerRing);

    sf::CircleShape inner(48.f);
    inner.setOrigin({48.f,48.f}); inner.setPosition({cx,cy});
    inner.setFillColor(sf::Color(8,25,90));
    inner.setOutlineColor(sf::Color(150,125,55));
    inner.setOutlineThickness(2.f); window.draw(inner);

    auto star8=[&](float x,float y,float r1,float r2,sf::Color col){
        sf::ConvexShape s(16);
        for(int i=0;i<16;i++){
            float a=i*3.14159f/8.f-3.14159f/2.f;
            float r=(i%2==0)?r1:r2;
            s.setPoint(i,{x+r*std::cos(a),y+r*std::sin(a)});
        }
        s.setFillColor(col); window.draw(s);
    };
    star8(cx,cy,40.f,17.f,sf::Color(190,155,65));
    star8(cx,cy,34.f,13.f,sf::Color(220,190,90));
    sf::CircleShape mdot(9.f);
    mdot.setOrigin({9.f,9.f}); mdot.setPosition({cx,cy});
    mdot.setFillColor(sf::Color::White); window.draw(mdot);

    sf::RectangleShape panel({500.f,360.f});
    panel.setOrigin({250.f,180.f}); panel.setPosition({cx,GC::H/2.f+30.f});
    panel.setFillColor(sf::Color(5,10,35,240));
    panel.setOutlineColor(sf::Color(200,165,70));
    panel.setOutlineThickness(3.f); window.draw(panel);

    sf::Text ucl(font,"UEFA CHAMPIONS LEAGUE",18);
    ucl.setFillColor(sf::Color(200,165,70)); ucl.setLetterSpacing(3.f);
    ucl.setPosition({cx-130.f,178.f}); window.draw(ucl);

    sf::Text tsh(font,"HEAD FOOTBALL",50);
    tsh.setFillColor(sf::Color(0,0,0,130));
    tsh.setPosition({cx-197.f,202.f}); window.draw(tsh);
    sf::Text ti(font,"HEAD FOOTBALL",50);
    ti.setFillColor(sf::Color(255,255,255));
    ti.setOutlineColor(sf::Color(200,165,70));
    ti.setOutlineThickness(2.5f);
    ti.setPosition({cx-199.f,200.f}); window.draw(ti);

    sf::RectangleShape ln({440.f,2.f});
    ln.setPosition({cx-220.f,258.f});
    ln.setFillColor(sf::Color(200,165,70)); window.draw(ln);

    auto btn=[&](const std::string& num,const std::string& txt,float y,sf::Color acc){
        sf::RectangleShape bg({440.f,54.f});
        bg.setOrigin({220.f,27.f}); bg.setPosition({cx,y+27.f});
        bg.setFillColor(sf::Color(8,15,50,230));
        bg.setOutlineColor(sf::Color(acc.r,acc.g,acc.b,160));
        bg.setOutlineThickness(1.5f); window.draw(bg);
        sf::RectangleShape bar({5.f,54.f});
        bar.setOrigin({2.5f,27.f}); bar.setPosition({cx-220.f,y+27.f});
        bar.setFillColor(acc); window.draw(bar);
        sf::Text nt(font,num,28); nt.setFillColor(acc);
        nt.setPosition({cx-208.f,y+10.f}); window.draw(nt);
        sf::Text tx(font,txt,24); tx.setFillColor(sf::Color(215,215,235));
        tx.setPosition({cx-160.f,y+13.f}); window.draw(tx);
    };
    btn("1","  2 Jucatori  (local)",278.f,sf::Color(200,165,70));
    btn("2","  vs Calculator (AI)", 342.f,sf::Color(100,175,255));
    btn("ESC","  Iesire",           406.f,sf::Color(140,140,160));

    sf::Text ctrl(font,"J1: A / D / W               J2: Sageti",19);
    ctrl.setFillColor(sf::Color(200,165,70));
    ctrl.setPosition({cx-185.f,468.f}); window.draw(ctrl);

    star8(cx-225.f,270.f,10.f,4.f,sf::Color(200,165,70,160));
    star8(cx+225.f,270.f,10.f,4.f,sf::Color(200,165,70,160));
}

void Game::drawGameOver(){
    sf::RectangleShape ov({GC::W,GC::H});
    ov.setFillColor(sf::Color(0,0,0,160)); window.draw(ov);

    sf::RectangleShape panel({500.f,310.f});
    panel.setOrigin({250.f,155.f}); panel.setPosition({GC::W/2.f,GC::H/2.f});
    panel.setFillColor(sf::Color(5,10,35,235));
    panel.setOutlineColor(sf::Color(200,165,70));
    panel.setOutlineThickness(3.f); window.draw(panel);

    std::string wm=player1->getScore()>player2->getScore()?player1->getName()+" castiga!":
                   player2->getScore()>player1->getScore()?player2->getName()+" castiga!":
                   "Egalitate!";
    sf::Text et(font,wm,42);
    et.setFillColor(sf::Color(200,165,70));
    et.setOutlineColor(sf::Color::Black); et.setOutlineThickness(2.f);
    et.setPosition({GC::W/2.f-200.f,GC::H/2.f-135.f}); window.draw(et);

    sf::Text si(font,
        player1->getName()+": "+std::to_string(player1->getScore())+
        "   "+player2->getName()+": "+std::to_string(player2->getScore()),30);
    si.setFillColor(sf::Color(80,220,80));
    si.setPosition({GC::W/2.f-200.f,GC::H/2.f-75.f}); window.draw(si);

    sf::Text rt(font,"R - Restart               M - Meniu",24);
    rt.setFillColor(sf::Color(180,255,180));
    rt.setPosition({GC::W/2.f-200.f,GC::H/2.f-10.f}); window.draw(rt);

    sf::Text st(font,
        "Bounce-uri: "+std::to_string(Ball::getTotalBounces())+
        "   Viteza max: "+std::to_string((int)Ball::getMaxSpeed())+" px/s",18);
    st.setFillColor(sf::Color(150,220,255));
    st.setPosition({GC::W/2.f-200.f,GC::H/2.f+58.f}); window.draw(st);
}