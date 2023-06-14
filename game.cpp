#include "game.h"
#include "resource_manager.h"
#include "sprite_renderer.h"
#include "game_object.h"
#include "ball_object.h"
#include<iostream>
#include<cmath>

// Game-related State data
SpriteRenderer* Renderer;
BallObject* Hole;
BallObject* Ball;
GameObject* Obs1_1, *Obs1_2, *Obs3;
GameObject* Arrow;
GameObject* PowerBar;

bool win;
bool displayInstructions;
double mod;

Game::Game(unsigned int width, unsigned int height)
    : State(GAME_ACTIVE), Keys(), Width(width), Height(height){}

Game::~Game()
{
    delete Renderer;
    delete Hole;
    delete Ball;
    delete Obs1_1;
    delete Obs1_2;
    delete Obs3;
    delete Arrow;
    delete PowerBar;
}


void Game::Init()
{
    // load shaders
    ResourceManager::LoadShader("shaders/sprite.vs", "shaders/sprite.frag", nullptr, "sprite");
    // configure shaders
    glm::mat4 projection = glm::ortho(0.0f, static_cast<float>(this->Width),
        static_cast<float>(this->Height), 0.0f, -1.0f, 1.0f);
    ResourceManager::GetShader("sprite").Use().SetInteger("image", 0);
    ResourceManager::GetShader("sprite").SetMatrix4("projection", projection);
    // set render-specific controls
    Shader spriteShader = ResourceManager::GetShader("sprite");
    Renderer = new SpriteRenderer(spriteShader);
    // load textures
    ResourceManager::LoadTexture("textures/menu.png", true, "menu");
    ResourceManager::LoadTexture("textures/win.png", true, "win");
    ResourceManager::LoadTexture("textures/background.png", true, "background");
    ResourceManager::LoadTexture("textures/ball.png", true, "ball");
    ResourceManager::LoadTexture("textures/obstacle.png", true, "obstacle");
    ResourceManager::LoadTexture("textures/obstacle1.png", true, "obstacle1");
    ResourceManager::LoadTexture("textures/obstacle2.png", true, "obstacle2");
    ResourceManager::LoadTexture("textures/arrow.png", true, "arrow");
    ResourceManager::LoadTexture("textures/hole.png", true, "hole");    
    ResourceManager::LoadTexture("textures/powerbar.png", true, "powerbar");
    //TEXTures
    ResourceManager::LoadTexture("textures/youwin.png", true, "youwin");
    ResourceManager::LoadTexture("textures/howto.png", true, "howto");
    ResourceManager::LoadTexture("textures/power.png", true, "power");
    // load levels
    displayInstructions = true; //display instruction sprite on first level
    this->Level = 0;            //set the first level
    this->State = GAME_MENU;    //set the start menu first
    glm::vec2 holePos = glm::vec2(300 - HOLE_SIZE, 100);                    //default hole position
    glm::vec2 ballPos = glm::vec2(300 - BALL_RADIUS, 700 - BALL_RADIUS);    //default ball position
    // configure game objects (LOADS FIRST LEVEL)
        //create hole target
    Hole = new BallObject(holePos, HOLE_SIZE, 10.0f, glm::vec2(0,0), ResourceManager::GetTexture("hole"));
    Hole->Color = glm::vec3(0.6, 0.6, 0.6);
        //create ball
    Ball = new BallObject(ballPos, BALL_RADIUS, BALL_MASS, glm::vec2(0,0), ResourceManager::GetTexture("ball"));
        //create obstacles
    Obs1_1 = new GameObject(glm::vec2(600,800), glm::vec2(300, 50), 0.0f, ResourceManager::GetTexture("obstacle1"));
    Obs1_2 = new GameObject(glm::vec2(600,800), glm::vec2(300, 50), 0.0f, ResourceManager::GetTexture("obstacle2"));
    Obs3 = new GameObject(glm::vec2(600, 800), glm::vec2(190, 10), 0.0f, ResourceManager::GetTexture("obstacle"));
    Obs3->Velocity = glm::vec2(-200.0, 0.0); //initial speed of moving obstacle
        //create direction arrow
    Arrow = new GameObject();
    Arrow->Sprite = ResourceManager::GetTexture("arrow");
    Arrow->Position = ballPos;
    Arrow->Size = glm::vec2(39, 145);
        //create power bar
    PowerBar = new GameObject(glm::vec3(18.0, 745.0, 1.0), glm::vec2(35.0, 300.0), 0, ResourceManager::GetTexture("powerbar"), glm::vec3(0.0,1.0,0.0));
}

void Game::Update(float dt) // LOOPS!
{
    // update objects
    Ball->Move(dt, this->Width, this->Height);      // update ball movement
    if (this->Level == 2) {
        Obs3->Move(dt, this->Width, this->Height);  // update obstacle movement on 3rd level
    }
    // check for collisions
    this->DoCollisions(dt);
    if (win) {                      // check win status after ball-hole collision
        win = false;                // reset win boolean
        Hole->Radius = 25.0f;     
        if (this->Level < 2) {      // if at first or second level, move to next
            this->Level++;
            ResetLevel();           // reset object positions
        }
        else if (this->Level == 2) {// if on last level show win screen
            this->State = GAME_WIN;
        }
    }
}

void Game::ProcessInput(float dt) // LOOPS!
{
    if (this->State == GAME_MENU) {         // if on menu screen, enter to start game
        if (this->Keys[GLFW_KEY_ENTER]) {
            this->State = GAME_ACTIVE;
        }
    }

    if (this->State == GAME_ACTIVE)
    {   mod=1.0;
        // configure velocities
        double xVel = (this->x1 - this->x2);    // x-coord difference
        double yVel = (this->y1 - this->y2);    // y-coord difference
        double maxL = 707.1068;                 // Shot power limit
        if (xVel > maxL) { xVel = maxL; }       // setting shot power limits
        if (xVel < -maxL) { xVel = -maxL; }
        if (yVel > maxL) { yVel = maxL; }
        if (yVel < -maxL) { yVel = -maxL; }
        double rad;
        
        // change levels
        if (this->Keys[GLFW_KEY_1])             // jump to Level 2
        {
            this->Level == 1;
            ResetLevel();
        }
        if (this->Keys[GLFW_KEY_2])             // jump to Level 3
        {
            this->Level == 2;
            ResetLevel();
        }
        
        if (this->Keys[GLFW_KEY_SPACE]) {       // reset current level
            ResetLevel();
        }

        if (this->rMouseClick) {
            
            Arrow->Position.x = Ball->Position.x - Arrow->Size.x / 2 + BALL_RADIUS;      //draw arrow sprite lined up with ball
            Arrow->Position.y = Ball->Position.y - Arrow->Size.y / 2 + BALL_RADIUS;
            
            double currX = (this->x - this->x1);        //use cursor position at click and current position to
            double currY = (this->y - this->y1);        //calculate angle of the arrow
            rad = atan2(currX, currY)*180/3.14159f;     //inverse tangent of 2 coord points converted to angles
            Arrow->Rotation = -rad;                     //rotate aim arrow based on angle

            double cXLim = currX;                       // same as velocity configuration above, but for constantly update value
            double cYLim = currY;                       // 
            if (currX > maxL) {  cXLim = maxL; }
            if (currX < -maxL) { cXLim = -maxL; }
            if (currY > maxL) {  cYLim = maxL; }
            if (currY < -maxL) { cYLim = -maxL; }
            double xPow2 = pow(cXLim, 2.0);             //annoying C6001 errors
            double yPow2 = pow(cYLim, 2.0);
            double power = sqrt(sqrt(xPow2+yPow2)/1000.0); //calc and display power of shot
            PowerBar->Color = glm::vec3(power, 1.0-power, 0.0);
            PowerBar->Position.y = 800.0 - 2.5*(power*100.0);
            PowerBar->Size.y = 0.0 + 2.5 * power * 100.0;
                                                                    
            // for checking values on console
            std::cout << "currX: "      << currX
                      << "\tcurrY: "    << currY
                      << "\trad: "      << rad
                      << "\tpower%: "   << power*100.0 << "%"
                      << "\tpowerC: "   << power
                      << "\tbarPos: "   << PowerBar->Position.y
                      << std::endl;
        }

        // mouse when released
        if (shoot == true) {
            Ball->Velocity.x = 1.25*xVel;   //modifying actual power
            Ball->Velocity.y = 1.25*yVel;
            Ball->Stuck = false;            //setting ball stopped state
            displayInstructions = false;    //remove instructions 
            shoot = false;                  //reset shot trigger bool
            std::cout << "ACTUAL POWER: " << sqrt((Ball->Velocity.x*Ball->Velocity.x)+(Ball->Velocity.y*Ball->Velocity.y)) << std::endl;
        }
}
    if (this->State == GAME_WIN) {          // if endgame reached
        if (this->Keys[GLFW_KEY_ENTER]) {   // reset to first level
            this->Level = 0;
            this->State = GAME_ACTIVE;
            ResetLevel();
        }
    }
}

void Game::Render() // WILL LOOP!
{

    if (this->State == GAME_MENU) {
        Texture2D menuTex = ResourceManager::GetTexture("menu");
        Renderer->DrawSprite(menuTex, glm::vec2(0.0f, 0.0f), glm::vec2(this->Width, this->Height), 0.0f);
        //RenderText()
    }
    
    if (this->State == GAME_ACTIVE)
    {
        // draw background
        Texture2D backgroundTex = ResourceManager::GetTexture("background");
        Renderer->DrawSprite(backgroundTex, glm::vec2(0.0f, 0.0f), glm::vec2(this->Width, this->Height), 0.0f);
        // draw player
        Hole->Draw(*Renderer);
        // draw ball
        Ball->Draw(*Renderer);
        // draw level
        if (this->Level == 0) {         // draw game instructions on first level
            if (displayInstructions) {
                Texture2D howtoText = ResourceManager::GetTexture("howto");
                Renderer->DrawSprite(howtoText, glm::vec2(20, 400), glm::vec2(283, 162), 0.0f);
            }
        }
        if (this->Level > 0) {          // draw obstacles based on level
            Obs1_1->Draw(*Renderer);
            Obs1_2->Draw(*Renderer);
        }
        if (this->Level == 2) {
            Obs3->Draw(*Renderer);
        }
        if (this->rMouseClick) {        // draw arrow and power bar during aiming
            Arrow->Draw(*Renderer);
            PowerBar->Draw(*Renderer);
            Texture2D powerText = ResourceManager::GetTexture("power");
            Renderer->DrawSprite(powerText, glm::vec3(5.0, 800.0-255.0f, 0.1), glm::vec2(60.0,255.0), 0.0f);
       }        
    }

    if (this->State == GAME_WIN) {      // draw end sprite
        Texture2D winTex = ResourceManager::GetTexture("win");
        Texture2D winTexText = ResourceManager::GetTexture("youwin");
        Renderer->DrawSprite(winTex, glm::vec2(0.0f, 0.0f), glm::vec2(this->Width, this->Height), 0.0f, glm::vec3(0.4, 0.4, 0.4));
        Renderer->DrawSprite(winTexText, glm::vec2(this->Width/2-130, 100), glm::vec2(260, 85), 0.0f);
    }
}


void Game::ResetLevel() // resets level
{
    //Initial Positions of Ball & Hole
    glm::vec2 holePos = glm::vec2(300 - HOLE_SIZE, 100);
    glm::vec2 ballPos = glm::vec2(300 - BALL_RADIUS, 700 - BALL_RADIUS);
    
    if (this->Level == 0) {         // FIRST LEVEL
        Ball->Position = ballPos;
        Hole->Position = holePos;
        displayInstructions = true;
        Obs1_1->Position = glm::vec2(600.0,800.0);
        Obs1_2->Position = glm::vec2(600.0,800.0);
        Obs3->Position = glm::vec2(600.0,800.0);
    }
    else if (this->Level == 1) {    // SECOND LEVEL
        Ball->Position = glm::vec2(this->Width * 2 / 3, 700 - BALL_RADIUS);
        Hole->Position = glm::vec2(this->Width*5/6, 50.0);    
        Obs1_1->Position = glm::vec2(0, this->Height / 3 + 125);
        Obs1_2->Position = glm::vec2(this->Width - 300, 150);
        Obs3->Position = glm::vec2(600.0,800.0);
    }
    else if (this->Level == 2) {    // THIRD LEVEL
        Ball->Position = ballPos;
        Hole->Position = holePos;
        Obs1_1->Position = glm::vec2(-75, 300);
        Obs1_2->Position = glm::vec2(375, 300);
        Obs3->Position = glm::vec2(150, 289);
    }
}

void Game::ResetBall() // resets ball position and velocity if stuck or despawned
{
    Ball->Position = glm::vec2(300-BALL_RADIUS, 700-BALL_RADIUS);
    Ball->Velocity = glm::vec2(0, 0);
}

// collision detection
double distanceSquared(int x1, int y1, int x2, int y2);
bool checkCollision(BallObject& a, BallObject& b);
bool CheckCollision(GameObject& one, GameObject& two);
Collision CheckCollision(BallObject& one, GameObject& two, int lol);
Collision CheckCollision(BallObject& one, GameObject& two);
Direction VectorDirection(glm::vec2 closest);

void Game::DoCollisions(float dt)
{
    glm::vec2 momentumVector = Ball->Velocity * Ball->Mass; // momentum vector to slow ball down
  
    Collision collision1 = CheckCollision(*Ball, *Obs1_1); // collision check with obstacle 1
    Collision collision2 = CheckCollision(*Ball, *Obs1_2); // collision check with obstacle 2
    Collision collision3 = CheckCollision(*Ball, *Obs3);   // collision check with obstacle 3 (moving)
    
    if (std::get<0>(collision1)) {  // Collision resolution for first obstacle
        std::cout << "BOX 1 HIT AT SPEED: " << Ball->Velocity.x << ", " << Ball->Velocity.y << std::endl;
        Direction dir = std::get<1>(collision1);
        glm::vec2 diff_vector = std::get<2>(collision1);
        if (dir == LEFT || dir == RIGHT) // horizontal collision
        {
            Ball->Velocity.x = -Ball->Velocity.x; // reverse horizontal velocity
            Ball->Velocity -= glm::normalize(momentumVector) * dt / BALL_MASS; // slow ball on each hit
            // relocate
            float penetration = Ball->Radius - std::abs(diff_vector.x);
            if (dir == LEFT)
                Ball->Position.x += penetration; // move ball to right
            else
                Ball->Position.x -= penetration; // move ball to left;
        }
        else // vertical collision
        {
            Ball->Velocity.y = -Ball->Velocity.y; // reverse vertical velocity
            Ball->Velocity -= glm::normalize(momentumVector) * dt / BALL_MASS;
            // relocate
            float penetration = Ball->Radius - std::abs(diff_vector.y);
            if (dir == UP)
                Ball->Position.y -= penetration; // move ball back up
            else
                Ball->Position.y += penetration; // move ball back down
        }
    }
    if (std::get<0>(collision2)) {   // Collision resolution for second obstacle
        std::cout << "BOX 2 HIT AT SPEED: " << Ball->Velocity.x << ", " << Ball->Velocity.y << std::endl;
        Direction dir = std::get<1>(collision2);
        glm::vec2 diff_vector = std::get<2>(collision2);
        if (dir == LEFT || dir == RIGHT) // horizontal collision
        {
            Ball->Velocity.x = -Ball->Velocity.x; // reverse horizontal velocity
            Ball->Velocity -= glm::normalize(momentumVector) * dt / BALL_MASS;
            // relocate
            float penetration = Ball->Radius - std::abs(diff_vector.x);
            if (dir == LEFT)
                Ball->Position.x += penetration; // move ball to right
            else
                Ball->Position.x -= penetration; // move ball to left;
        }
        else // vertical collision
        {
            Ball->Velocity.y = -Ball->Velocity.y; // reverse vertical velocity
            Ball->Velocity -= glm::normalize(momentumVector) * dt / BALL_MASS;
            // relocate
            float penetration = Ball->Radius - std::abs(diff_vector.y);
            if (dir == UP)
                Ball->Position.y -= penetration; // move ball back up
            else
                Ball->Position.y += penetration; // move ball back down
        }
    }
    if (std::get<0>(collision3)) {      // Collision resolution for third obstacle (moving)
        std::cout << "BOX 3 HIT AT SPEED: " << Ball->Velocity.x << ", " << Ball->Velocity.y << std::endl;
        Direction dir = std::get<1>(collision3);
        glm::vec2 diff_vector = std::get<2>(collision3);
        if (dir == LEFT || dir == RIGHT) // horizontal collision
        {
            Ball->Velocity.x = -Ball->Velocity.x; // reverse horizontal velocity
            Ball->Velocity -= glm::normalize(momentumVector) * dt / BALL_MASS;            
            float penetration = Ball->Radius - std::abs(diff_vector.x);
            if (dir == LEFT) {
                Ball->Position.x += 2.0*penetration; // move ball to 
            }
            else {
                Ball->Position.x -= 2.0*penetration; // move ball to 
            }
        }
        else // vertical collision
        {
            Ball->Velocity.y = -Ball->Velocity.y; // reverse vertical velocity
            Ball->Velocity -= glm::normalize(momentumVector) * dt / BALL_MASS;
            // relocate
            float penetration = Ball->Radius - std::abs(diff_vector.y);
            if (dir == UP)
                Ball->Position.y -= penetration; // move ball bback up
            else
                Ball->Position.y += penetration; // move ball back down
        }
    }
    
    bool result = checkCollision(*Ball, *Hole);    // Collision resolution if ball touches hole
    if (!Ball->Stuck && result)
    {
        float time = 1.0;           // pause before moving to next level
        while (time > 0.0f) {
            std::cout << "Time: " << time << "\t" << Ball->Position.x << ", " << Ball->Position.y << std::endl;
            time -= 1.0 / 10.0 * dt;
        }
        //Ball->Position.x = Player->Position.x - BALL_RADIUS + HOLE_SIZE;  // set ball to hole animation (NOT USED)
        //Ball->Position.y = Player->Position.y - BALL_RADIUS + HOLE_SIZE;
        Ball->Velocity.x = 0;   //reset speed
        Ball->Velocity.y = 0; 
        Ball->Stuck==true;      //set ball to stop
        win = true;             //set win state
        ResetBall();            //reset ball position
    }
}
double distanceSquared(int x1, int y1, int x2, int y2)
{
    int deltaX = x2 - x1;
    int deltaY = y2 - y1;
    return deltaX * deltaX + deltaY * deltaY;
}
bool checkCollision(BallObject& a, BallObject& b) // COLLISION CHECK FOR BALL & HOLE
{
    //Calculate total radius squared
    int totalRadiusSquared = a.Radius + (b.Radius-6.0f);
    totalRadiusSquared = totalRadiusSquared * totalRadiusSquared;

    //If the distance between the centers of the circles is less than the sum of their radii
    if (distanceSquared(a.Position.x, a.Position.y, b.Position.x, b.Position.y) < (totalRadiusSquared))
    {
        //The circles have collided
        return true;
    }
    //If not
    return false;
}

bool CheckCollision(GameObject& one, GameObject& two) // AABB - AABB collision
{
    // collision x-axis?
    bool collisionX = one.Position.x + one.Size.x >= two.Position.x &&
        two.Position.x + two.Size.x >= one.Position.x;
    // collision y-axis?
    bool collisionY = one.Position.y + one.Size.y >= two.Position.y &&
        two.Position.y + two.Size.y >= one.Position.y;
    // collision only if on both axes
    return collisionX && collisionY;
}

Collision CheckCollision(BallObject& one, GameObject& two) // AABB - Circle collision
{
    // get center point circle first 
    glm::vec2 center(one.Position + one.Radius);
    // calculate AABB info (center, half-extents)
    glm::vec2 aabb_half_extents(two.Size.x / 2.0f, two.Size.y / 2.0f);
    glm::vec2 aabb_center(two.Position.x + aabb_half_extents.x, two.Position.y + aabb_half_extents.y);
    // get difference vector between both centers
    glm::vec2 difference = center - aabb_center;
    glm::vec2 clamped = glm::clamp(difference, -aabb_half_extents, aabb_half_extents);
    // now that we know the the clamped values, add this to AABB_center and we get the value of box closest to circle
    glm::vec2 closest = aabb_center + clamped;
    // now retrieve vector between center circle and closest point AABB and check if length < radius
    difference = closest - center;

    if (glm::length(difference) < one.Radius) // not <= since in that case a collision also occurs when object one exactly touches object two, which they are at the end of each collision resolution stage.
        return std::make_tuple(true, VectorDirection(difference), difference);
    else
        return std::make_tuple(false, UP, glm::vec2(0.0f, 0.0f));
}

// calculates which direction a vector is facing (N,E,S or W)
Direction VectorDirection(glm::vec2 target)
{
    glm::vec2 compass[] = {
        glm::vec2(0.0f, 1.0f),	// up
        glm::vec2(1.0f, 0.0f),	// right
        glm::vec2(0.0f, -1.0f),	// down
        glm::vec2(-1.0f, 0.0f)	// left
    };
    float max = 0.0f;
    unsigned int best_match = -1;
    for (unsigned int i = 0; i < 4; i++)
    {
        float dot_product = glm::dot(glm::normalize(target), compass[i]);
        if (dot_product > max)
        {
            max = dot_product;
            best_match = i;
        }
    }
    return (Direction)best_match;
}