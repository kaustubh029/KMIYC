#include "gui.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include "encrdec.h"

//~~~~~~~~~~~~~
// Game Defines
//~~~~~~~~~~~~~
#define MAX_BUILDINGS 2500
#define PLAYER_MAX_SHOOTS 50
#define ENEMY1_MAX_SHOOTS 50
#define ENEMY3_MAX_SHOOTS 50

//~~~~~~~~~~~~~~~~~~~~~~
// Structures Definition
//~~~~~~~~~~~~~~~~~~~~~~
typedef struct Buildings
{
    Vector2 position;
    Vector2 size;
    Color color;
} Buildings;

typedef struct Player
{
    Vector2 position;
    Vector2 speed;
    float width;                                 // width of player sprite
    float height;                                // height of player sprite
    float health;
    Rectangle head;
    Rectangle body;
    bool direction;                              // TURE if moving forward FALSE if moving backward
} Player;

typedef struct Gun
{
    Vector2 centerWrtGun;                        // point wrt which gun can rotate [WRT GUN SPRITE TOP LEFT CONNER]
    Vector2 centerWrtBody;                       // point wrt which gun can rotate [WRT PLAYER SPRITE TOP LEFT CONNER]
    float width;                                 // width of gun sprite
    float height;                                // height of gun sprite
    double rotationDegree;
    float damage;
} Gun;

typedef struct Shoot
{
    Vector2 position;
    Vector2 speed;
    Vector2 size;
    double rotationDegree;
    Color color;
    bool active;
} Shoot;

typedef struct Enemy
{
    Vector2 position;
    Vector2 speed;
    Vector2 aim;
    float width;                                 // width of enemy sprite
    float height;                                // height of enemy sprite
    float health;
    float shield;
    float shootTimer;
    Rectangle head;
    Rectangle body;
    bool active;
    bool direction;                              // True if forward, False if backward
} Enemy;

typedef struct EnemyShootingAccuracy
{
    Rectangle level1;
    Rectangle level2;
    Rectangle level3;
} EnemyShootingAccuracy;

typedef struct Objects
{
    Rectangle energyDrink;
    Rectangle adrenalineSyringe;
    int energyDrinkCount;
    int adrenalineSyringeCount;
    float energyDrinkTimer;
    bool energyDrinkActive;
    bool energyDrinkInUse;
    bool adrenalineSyringeActive;
} Objects;

typedef struct Sounds_gun
{
    Sound sound1;
    Sound bullethit;
} Sounds_gun;

// Textures
typedef struct Run
{
    Texture2D spriteSheet;
} Run;

typedef struct Idle
{
    Texture2D spriteSheet;
} Idle;

typedef struct Walk
{
    Texture2D spriteSheet;
} Walk;

typedef struct Jump
{
    Texture2D spriteSheet;
} Jump;

typedef struct Texture_player
{
    Run run;
    Run run180;
    Idle idle;
    Idle idle180;
    Jump jump;
    Jump jump180;
    Walk walk;
    Walk walk180;
} Texture_player;

typedef struct Texture_Enemy1
{
    Idle idle;
    Idle idle180;
} Texture_Enemy1;

typedef struct Texture_enemy2
{
    Run run;
} Texture_enemy2;

typedef struct Texture_enemy3
{
    Walk walk;
    Walk walk180;
} Texture_enemy3;

typedef struct Texture_weapons
{
    Texture gun;
    Texture2D gun180;
} Texture_weapons;

typedef struct Texture_objects
{
    Texture energyDrink;
    Texture adrenalineSyringe;
} Texture_objects;

typedef struct Texture_symbols
{
    Texture heart;
    Texture airtime;
    Texture enerydrink;
    Texture adrenalinesyringe;
} Texture_symbols;

typedef struct Texture_others
{
    Texture mainscreen;
    Texture menuscreen;
    Texture playButton;
    Texture highestscoreButton;
    Texture helpButton;
    Texture exitButton;
    Texture resumeButton;
    Texture quitButton;
    Texture soundOnButton;
    Texture soundOffButton;
    Texture helpscreen;
    Texture gameoverscreen;
} Texture_others;

//~~~~~~~~~~~~~~~~
// Global Variables
//~~~~~~~~~~~~~~~~
static const int screenWidth = 1920;
static const int screenHeight = 900;

static int score = 0;
static int time;
static int highestscore;
static bool gameover;
static bool pause;
static bool quit;
static bool sound;
static bool exitGame;

static Buildings building[MAX_BUILDINGS];
static Player player = {0};

static Enemy enemy1 = {0};
static Enemy enemy2 = {0};
static Enemy enemy3 = {0};

static Gun player_gun = {0};
static Gun enemy1_gun = {0};
static Gun enemy3_gun = {0};

static Shoot shoot[PLAYER_MAX_SHOOTS] = {0};
static Shoot enemy1_shoot[ENEMY1_MAX_SHOOTS] = {0};
static Shoot enemy3_shoot[ENEMY3_MAX_SHOOTS] = {0};

static EnemyShootingAccuracy enemyShootingAccuracy = {0};
static Objects objects = {0};
static Sounds_gun sounds_gun;

Camera2D camera = {0};
static Vector2 mousePosition;

static char scorestring[10];
FILE *filepointer;

static int gravitySpeedConst = 600;
static bool player_grounded = true;
static int levelUpCounter = 120;

static int active_building;
static int enemy1_active_building;
static int enemy3_active_building;

static int enemy1KillPoints;
static int enemy2KillPoints;
static int enemy3KillPoints;

static float redline_counter;
static float redline_x;

static int enemy3turn = -1;
static float redline_counter;
static float redline_x;

static float player_healing_timer = 0;

static bool enemy_create = false;
static int enemy_create_counter = 10;
static int object_create_counter = 15;

static float airTime = 100;
static float airTimer = 0;

static float player_animation_timer = 0.0f;
static int player_animation_Currentframe = 0;
static int player_animation_MaxFrames;

static float enemy1_animation_timer = 0.0f;
static int enemy1_animation_Currentframe = 0;
static int enemy1_animation_MaxFrames;

static float enemy2_animation_timer = 0.0f;
static int enemy2_animation_Currentframe = 0;
static int enemy2_animation_MaxFrames;

static float enemy3_animation_timer = 0.0f;
static int enemy3_animation_Currentframe = 0;
static int enemy3_animation_MaxFrames;

Texture_player texture_player;
Texture_weapons texture_weapons;
Texture_Enemy1 texture_enemy1;
Texture_enemy2 texture_enemy2;
Texture_enemy3 texture_enemy3;
Texture_objects texture_objects;
Texture_symbols texture_symbols;
Texture_others texture_others;

//~~~~~~~~~~~~~~~~~~~~~~
// Functions Declaration
//~~~~~~~~~~~~~~~~~~~~~~
static int Init();
static int Game();
static int GameLogic();
static void DrawGame();

static int LoadGameTextures();
static int UnloadGameTextures();
static int LoadGameSounds();
static int UnloadGameSounds();

static int MainScreen();
static int MenuScreen();
static int HelpScreen();
static int HighestScoreScreen();
static int GameOverScreen();
static int PauseScreen();

static int FetchData();
static int FeedData();

int main()
{
    InitWindow(screenWidth, screenHeight, "KMIYC");
    SetExitKey(KEY_END);
    InitAudioDevice();
    LoadGameTextures();
    LoadGameSounds();
    MainScreen();
    MenuScreen();
    UnloadGameTextures();
    UnloadGameSounds();
    CloseAudioDevice();
    CloseWindow();
    return 0;
}

int Game()
{
    // Main game loop
    while (true)
    {
        if (WindowShouldClose() || exitGame)
        {
            exitGame = true;
            break;
        }
        if (gameover)
        {
            break;
        }
        if (pause)
        {
            ShowCursor();
            PauseScreen();
            if (quit)
            {
                break;
            }
        }
        DrawGame();
        GameLogic();
        // gamesound();
    }
    return 0;
}

int GameLogic()
{
    if (IsKeyPressed(KEY_ESCAPE))
    {
        pause = true;
    }
    if (!pause)     // Don't really need this condition becoz the while loop of pause screen is acting as a clutch
    {
        // Time
        time = (int)GetTime() + 1;

        // Update mouse coordinates wrt the world instead of screen
        mousePosition = GetScreenToWorld2D(GetMousePosition(), camera);

        // Player Movements
        if ((IsKeyDown(KEY_D) || IsKeyDown(KEY_RIGHT)))
        {
            // Collision with vertical wall logic
            if (!((player.position.x + player.width >= building[active_building + 1].position.x) && (player.position.y + player.height > building[active_building + 1].position.y)))
            {
                player.position.x += player.speed.x * GetFrameTime();
                camera.target.x += player.speed.x * GetFrameTime();
            }

            player.direction = true;
        }

        if ((IsKeyDown(KEY_A) || IsKeyDown(KEY_LEFT)))
        {
            // Collision with vertical wall logic
            if (!((player.position.x <= building[active_building - 1].position.x + building[active_building - 1].size.x) && (player.position.y + player.height > building[active_building - 1].position.y)))
            {
                player.position.x -= player.speed.x * GetFrameTime();
                camera.target.x -= player.speed.x * GetFrameTime();
            }

            player.direction = false;
        }

        if ((IsKeyDown(KEY_LEFT_SHIFT)) && !(IsKeyDown(KEY_A) || IsKeyDown(KEY_LEFT)) && !(IsKeyDown(KEY_D) || IsKeyDown(KEY_RIGHT)))
        {
            if (player.direction)
            {
                // Collision with vertical wall logic
                if (!((player.position.x + player.width >= building[active_building + 1].position.x) && (player.position.y + player.height > building[active_building + 1].position.y)))
                {
                    player.position.x += player.speed.x * GetFrameTime() * 1.5;
                    camera.target.x += player.speed.x * GetFrameTime() * 1.5;
                }
            }
            else
            {
                // Collision with vertical wall logic
                if (!((player.position.x <= building[active_building - 1].position.x + building[active_building - 1].size.x) && (player.position.y + player.height > building[active_building - 1].position.y)))
                {
                    player.position.x -= player.speed.x * GetFrameTime() * 1.5;
                    camera.target.x -= player.speed.x * GetFrameTime() * 1.5;
                }
            }
        }

        if (airTime > 0)
        {
            if ((IsKeyDown(KEY_W) || IsKeyDown(KEY_UP)) && player.position.y > 0)
            {
                player.position.y -= player.speed.y * GetFrameTime();
                airTime -= GetFrameTime() * 10;
            }
            if ((IsKeyDown(KEY_S) || IsKeyDown(KEY_DOWN)) && !player_grounded)
            {
                player.position.y += player.speed.y * GetFrameTime();
                airTime -= GetFrameTime() * 10;
            }
        }

        // Player airtime logic
        if (player_grounded)
        {
            airTimer += GetFrameTime();
            if (airTimer > 1.25f && airTime < 100)
            {
                if (airTime > 90)
                    airTime = 100;
                else
                    airTime += 10;
                airTimer = 0;
            }
        }

        // Updating player variables
        player.head = (Rectangle){player.position.x + 8, player.position.y + 11, 64, 60};
        player.body = (Rectangle){player.position.x + 18, player.position.y + 71, 45, 33};

        // Active building logic for player
        for (int i = 0; i < MAX_BUILDINGS; i++)
        {
            if (player.direction)
            {
                if ((player.position.x + (player.width * 0.7) > building[i].position.x) && (player.position.x + (player.width * 0.7) < building[i].position.x + building[i].size.x))
                {
                    active_building = i;
                    break;
                }
            }
            else
            {
                if ((player.position.x + (player.width * 0.5) < building[i].position.x + building[i].size.x) && (player.position.x + (player.width * 0.5) > building[i].position.x))
                {
                    active_building = i;
                    break;
                }
            }
        }

        // Gravity logic for player
        if ((player.position.y + player.height) < (screenHeight - building[active_building].size.y))
        {
            player.position.y += gravitySpeedConst * GetFrameTime();
        }

        // Player grounded logic
        if (player.position.y + player.height < building[active_building].position.y)
        {
            player_grounded = false;
        }
        else
        {
            player_grounded = true;
        }

        // Updating gun variables
        if (player.direction)
        {
            player_gun.centerWrtGun.x = 42;
            player_gun.centerWrtBody.x = 4;
        }
        else
        {
            player_gun.centerWrtGun.x = player_gun.width - 42;
            player_gun.centerWrtBody.x = -51;
        }

        // Gun rotation logic
        player_gun.rotationDegree = atan2f((mousePosition.y - GetScreenToWorld2D((Vector2){screenWidth / 2, screenHeight / 2}, camera).y), (mousePosition.x - GetScreenToWorld2D((Vector2){screenWidth / 2, screenHeight / 2}, camera).x)) * (180.0f / PI);
        if (player_gun.rotationDegree < 0)
            player_gun.rotationDegree += 360.0f;

        // Player shoot logic
        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
        {
            for (int i = 0; i < PLAYER_MAX_SHOOTS; i++)
            {
                if (!shoot[i].active)
                {
                    shoot[i].active = true;
                    if (sound)
                        PlaySound(sounds_gun.sound1);
                    if (player.direction)
                        shoot[i].position = (Vector2){player.position.x + player_gun.centerWrtBody.x + player_gun.centerWrtGun.x + (player_gun.width - player_gun.centerWrtGun.x) * cos(player_gun.rotationDegree * DEG2RAD), player.position.y + player_gun.centerWrtBody.y + player_gun.centerWrtGun.y + (player_gun.width - player_gun.centerWrtGun.x) * sin(player_gun.rotationDegree * DEG2RAD) - shoot[i].size.y};
                    else
                        shoot[i].position = (Vector2){player.position.x + player_gun.centerWrtBody.x + player_gun.centerWrtGun.x - (player_gun.centerWrtGun.x) * cos((player_gun.rotationDegree + 180) * DEG2RAD), player.position.y + player_gun.centerWrtBody.y + player_gun.centerWrtGun.y - (player_gun.centerWrtGun.x) * sin((player_gun.rotationDegree + 180) * DEG2RAD) - shoot[i].size.y};
                    shoot[i].speed.x = cos(player_gun.rotationDegree * DEG2RAD) * GetFrameTime() * 5600;
                    shoot[i].speed.y = sin(player_gun.rotationDegree * DEG2RAD) * GetFrameTime() * 5600;
                    shoot[i].rotationDegree = player_gun.rotationDegree;
                    break;
                }
            }
        }

        // Shoot logic
        for (int i = 0; i < PLAYER_MAX_SHOOTS; i++)
        {

            if (shoot[i].active)
            {
                // Shoot deactive logic
                if (shoot[i].position.x > player.position.x + screenWidth || shoot[i].position.y < 0 || shoot[i].position.x < player.position.x - screenWidth || CheckCollisionRecs((Rectangle){shoot[i].position.x, shoot[i].position.y, shoot[i].size.x, shoot[i].size.y}, (Rectangle){building[active_building].position.x, building[active_building].position.y, building[active_building].size.x, building[active_building].size.y}) || CheckCollisionRecs((Rectangle){shoot[i].position.x, shoot[i].position.y, shoot[i].size.x, shoot[i].size.y}, (Rectangle){building[active_building + 1].position.x, building[active_building + 1].position.y, building[active_building + 1].size.x, building[active_building + 1].size.y}) || CheckCollisionRecs((Rectangle){shoot[i].position.x, shoot[i].position.y, shoot[i].size.x, shoot[i].size.y}, (Rectangle){building[active_building + 2].position.x, building[active_building + 2].position.y, building[active_building + 2].size.x, building[active_building + 2].size.y}) || CheckCollisionRecs((Rectangle){shoot[i].position.x, shoot[i].position.y, shoot[i].size.x, shoot[i].size.y}, (Rectangle){building[active_building + 3].position.x, building[active_building + 3].position.y, building[active_building + 3].size.x, building[active_building + 3].size.y}) || CheckCollisionRecs((Rectangle){shoot[i].position.x, shoot[i].position.y, shoot[i].size.x, shoot[i].size.y}, (Rectangle){building[active_building - 1].position.x, building[active_building - 1].position.y, building[active_building - 1].size.x, building[active_building - 1].size.y}) || CheckCollisionRecs((Rectangle){shoot[i].position.x, shoot[i].position.y, shoot[i].size.x, shoot[i].size.y}, (Rectangle){building[active_building - 2].position.x, building[active_building - 2].position.y, building[active_building - 2].size.x, building[active_building - 2].size.y}) || CheckCollisionRecs((Rectangle){shoot[i].position.x, shoot[i].position.y, shoot[i].size.x, shoot[i].size.y}, (Rectangle){building[active_building - 3].position.x, building[active_building - 3].position.y, building[active_building - 3].size.x, building[active_building - 3].size.y}))
                {
                    shoot[i].active = false;
                }

                shoot[i].position.x += shoot[i].speed.x;
                shoot[i].position.y += shoot[i].speed.y;
            }
        }

        // Player healing logic
        player_healing_timer += GetFrameTime();
        if (player_healing_timer > 2.0f && player.health < 100)
        {
            if (player.health > 80)
                player.health = 100;
            else
                player.health += 20;
            player_healing_timer = 0;
        }

        // Checking if player shoot is colliding with enemies
        if (enemy1.active)
        {
            for (int i = 0; i < PLAYER_MAX_SHOOTS; i++)
            {
                if (shoot[i].active)
                {
                    if (CheckCollisionRecs((Rectangle){shoot[i].position.x, shoot[i].position.y, shoot[i].size.x, shoot[i].size.y}, enemy1.head) || CheckCollisionRecs((Rectangle){shoot[i].position.x, shoot[i].position.y, shoot[i].size.x, shoot[i].size.y}, enemy1.body))
                    {
                        shoot[i].active = false;
                        if (sound)
                            PlaySound(sounds_gun.bullethit);
                        enemy1.health = enemy1.health + enemy1.shield - player_gun.damage;
                        if (enemy1.health <= 0)
                        {
                            score += enemy1KillPoints;
                            enemy1.active = false;
                            enemy1.health = 100;
                        }
                    }
                }
            }
        }

        if (enemy2.active)
        {
            for (int i = 0; i < PLAYER_MAX_SHOOTS; i++)
            {
                if (shoot[i].active)
                {
                    if (CheckCollisionRecs((Rectangle){shoot[i].position.x, shoot[i].position.y, shoot[i].size.x, shoot[i].size.y}, enemy2.head))
                    {
                        shoot[i].active = false;
                        if (sound)
                            PlaySound(sounds_gun.bullethit);
                        enemy2.health = enemy2.health + enemy2.shield - player_gun.damage;
                        if (enemy2.health <= 0)
                        {
                            score += enemy2KillPoints;
                            enemy2.active = false;
                            enemy2.health = 100;
                        }
                    }
                    if (CheckCollisionRecs((Rectangle){shoot[i].position.x, shoot[i].position.y, shoot[i].size.x, shoot[i].size.y}, enemy2.body))
                    {
                        shoot[i].active = false;
                        enemy2.health = enemy2.health - player_gun.damage;
                        if (enemy2.health <= 0)
                        {
                            score += enemy2KillPoints;
                            enemy2.active = false;
                            enemy2.health = 100;
                        }
                    }
                }
            }
        }

        if (enemy3.active)
        {
            for (int i = 0; i < PLAYER_MAX_SHOOTS; i++)
            {
                if (shoot[i].active)
                {
                    if (CheckCollisionRecs((Rectangle){shoot[i].position.x, shoot[i].position.y, shoot[i].size.x, shoot[i].size.y}, enemy3.head) || CheckCollisionRecs((Rectangle){shoot[i].position.x, shoot[i].position.y, shoot[i].size.x, shoot[i].size.y}, enemy3.body))
                    {
                        shoot[i].active = false;
                        if (sound)
                            PlaySound(sounds_gun.bullethit);
                        enemy3.health = enemy3.health + enemy3.shield - player_gun.damage;
                        if (enemy3.health <= 0)
                        {
                            score += enemy3KillPoints;
                            enemy3.active = false;
                            enemy3.health = 100;
                        }
                    }
                }
            }
        }

        // Enemy random choose
        enemy_create = false;
        if (time % enemy_create_counter == 0)
        {
            enemy_create = true;
            enemy_create_counter += 10;
        }

        // Enemy1 active logic
        if (enemy_create)
        {
            if (!enemy1.active)
            {
                enemy1.active = true;
                enemy1.position.x = player.position.x + GetRandomValue(screenWidth, screenWidth * 5);
                enemy1.position.y = building[(int)(enemy1.position.x / building[0].size.x)].position.y - enemy1.height;
            }
        }

        // Updating enemyShootingAccuracy
        enemyShootingAccuracy.level1 = (Rectangle){player.position.x, player.position.y, player.width, player.height};
        enemyShootingAccuracy.level2 = (Rectangle){player.position.x, player.position.y + 1, player.width, 90};
        enemyShootingAccuracy.level2 = (Rectangle){player.position.x + 4, player.position.y + 6, 71, 61};

        // Enemy1 shoot active logic
        if (enemy1.active)
        {
            // Updating enemy1 variables
            enemy1.head = (Rectangle){enemy1.position.x + 8, enemy1.position.y + 11, 64, 60};
            enemy1.body = (Rectangle){enemy1.position.x + 18, enemy1.position.y + 71, 45, 33};

            enemy1_active_building = (int)((enemy1.position.x + enemy1.width / 2) / building[0].size.x);

            if (player.position.x + player.width / 2 > enemy1.position.x + enemy1.width / 2)
                enemy1.direction = true;
            else
                enemy1.direction = false;

            // Updating gun variables for enenmy1
            if (enemy1.direction)
            {
                enemy1_gun.centerWrtGun.x = 42;
                enemy1_gun.centerWrtBody.x = 4;
            }
            else
            {
                enemy1_gun.centerWrtGun.x = enemy1_gun.width - 42;
                enemy1_gun.centerWrtBody.x = -51;
            }

            enemy1.shootTimer += GetFrameTime();


            if (enemy1.shootTimer >= 0.5f)
            {
                enemy1.shootTimer = 0;
                enemy1.aim = (Vector2){GetRandomValue(enemyShootingAccuracy.level1.x, enemyShootingAccuracy.level1.x + enemyShootingAccuracy.level1.width), GetRandomValue(enemyShootingAccuracy.level1.y, enemyShootingAccuracy.level1.y + enemyShootingAccuracy.level1.height)};

                // Enemy1 gun rotation logic
                enemy1_gun.rotationDegree = (atan2f((enemy1.aim.y - (enemy1.position.y + enemy1_gun.centerWrtBody.y + enemy1_gun.centerWrtGun.y)), (enemy1.aim.x - (enemy1.position.x + enemy1_gun.centerWrtBody.x + enemy1_gun.centerWrtGun.x))) * (180.0f / PI));
                if (enemy1_gun.rotationDegree < 0)
                    enemy1_gun.rotationDegree += 360.0f;

                for (int i = 0; i < ENEMY1_MAX_SHOOTS && player.position.x > enemy1.position.x - screenWidth * 2; i++)
                {
                    if (!enemy1_shoot[i].active)
                    {
                        enemy1_shoot[i].active = true;
                        if (enemy1.direction)
                            enemy1_shoot[i].position = (Vector2){enemy1.position.x + enemy1_gun.centerWrtBody.x + enemy1_gun.centerWrtGun.x + (enemy1_gun.width - enemy1_gun.centerWrtGun.x) * cos((enemy1_gun.rotationDegree) * DEG2RAD), enemy1.position.y + enemy1_gun.centerWrtBody.y + enemy1_gun.centerWrtGun.y + (enemy1_gun.width - enemy1_gun.centerWrtGun.x) * sin((enemy1_gun.rotationDegree) * DEG2RAD)};
                        else
                            enemy1_shoot[i].position = (Vector2){enemy1.position.x + enemy1_gun.centerWrtBody.x + enemy1_gun.centerWrtGun.x - (enemy1_gun.centerWrtGun.x) * cos((enemy1_gun.rotationDegree + 180) * DEG2RAD), enemy1.position.y + enemy1_gun.centerWrtBody.y + enemy1_gun.centerWrtGun.y - (enemy1_gun.centerWrtGun.x) * sin((enemy1_gun.rotationDegree + 180) * DEG2RAD)};
                        enemy1_shoot[i].speed.x = cos((enemy1_gun.rotationDegree) * DEG2RAD) * GetFrameTime() * 5600;
                        enemy1_shoot[i].speed.y = sin((enemy1_gun.rotationDegree) * DEG2RAD) * GetFrameTime() * 5600;
                        enemy1_shoot[i].rotationDegree = enemy1_gun.rotationDegree;
                        break;
                    }
                }
            }

            // Enemy1 shooting logic
            for (int i = 0; i < ENEMY1_MAX_SHOOTS; i++)
            {

                if (enemy1_shoot[i].active)
                {
                    if (enemy1_shoot[i].position.x > enemy1.position.x + screenWidth * 3 || enemy1_shoot[i].position.y < 0 || enemy1_shoot[i].position.x < enemy1.position.x - screenWidth * 3 || CheckCollisionRecs((Rectangle){enemy1_shoot[i].position.x, enemy1_shoot[i].position.y, enemy1_shoot[i].size.x, enemy1_shoot[i].size.y}, (Rectangle){building[enemy1_active_building].position.x, building[enemy1_active_building].position.y, building[enemy1_active_building].size.x, building[enemy1_active_building].size.y}) || CheckCollisionRecs((Rectangle){enemy1_shoot[i].position.x, enemy1_shoot[i].position.y, enemy1_shoot[i].size.x, enemy1_shoot[i].size.y}, (Rectangle){building[enemy1_active_building + 1].position.x, building[enemy1_active_building + 1].position.y, building[enemy1_active_building + 1].size.x, building[enemy1_active_building + 1].size.y}) || CheckCollisionRecs((Rectangle){enemy1_shoot[i].position.x, enemy1_shoot[i].position.y, enemy1_shoot[i].size.x, enemy1_shoot[i].size.y}, (Rectangle){building[enemy1_active_building + 2].position.x, building[enemy1_active_building + 2].position.y, building[enemy1_active_building + 2].size.x, building[enemy1_active_building + 2].size.y}) || CheckCollisionRecs((Rectangle){enemy1_shoot[i].position.x, enemy1_shoot[i].position.y, enemy1_shoot[i].size.x, enemy1_shoot[i].size.y}, (Rectangle){building[enemy1_active_building + 3].position.x, building[enemy1_active_building + 3].position.y, building[enemy1_active_building + 3].size.x, building[enemy1_active_building + 3].size.y}) || CheckCollisionRecs((Rectangle){enemy1_shoot[i].position.x, enemy1_shoot[i].position.y, enemy1_shoot[i].size.x, enemy1_shoot[i].size.y}, (Rectangle){building[enemy1_active_building - 1].position.x, building[enemy1_active_building - 1].position.y, building[enemy1_active_building - 1].size.x, building[enemy1_active_building - 1].size.y}) || CheckCollisionRecs((Rectangle){enemy1_shoot[i].position.x, enemy1_shoot[i].position.y, enemy1_shoot[i].size.x, enemy1_shoot[i].size.y}, (Rectangle){building[enemy1_active_building - 2].position.x, building[enemy1_active_building - 2].position.y, building[enemy1_active_building - 2].size.x, building[enemy1_active_building - 2].size.y}) || CheckCollisionRecs((Rectangle){enemy1_shoot[i].position.x, enemy1_shoot[i].position.y, enemy1_shoot[i].size.x, enemy1_shoot[i].size.y}, (Rectangle){building[enemy1_active_building - 3].position.x, building[enemy1_active_building - 3].position.y, building[enemy1_active_building - 3].size.x, building[enemy1_active_building - 3].size.y}))
                    {
                        enemy1_shoot[i].active = false;
                    }

                    if (CheckCollisionRecs((Rectangle){enemy1_shoot[i].position.x, enemy1_shoot[i].position.y, enemy1_shoot[i].size.x, enemy1_shoot[i].size.y}, player.head) || CheckCollisionRecs((Rectangle){enemy1_shoot[i].position.x, enemy1_shoot[i].position.y, enemy1_shoot[i].size.x, enemy1_shoot[i].size.y}, player.body))
                    {
                        enemy1_shoot[i].active = false;
                        if (sound)
                            PlaySound(sounds_gun.bullethit);

                        if (!objects.energyDrinkInUse)
                        {
                            player.health = player.health - enemy1_gun.damage;
                        }
                    }
                }
            }
        }

        // Enemy1 shoot movement update
        for (int i = 0; i < ENEMY1_MAX_SHOOTS; i++)
        {
            if (enemy1_shoot[i].active)
            {
                enemy1_shoot[i].position.x += enemy1_shoot[i].speed.x;
                enemy1_shoot[i].position.y += enemy1_shoot[i].speed.y;
            }
        }

        // Enemy3 active logic
        if (enemy_create)
        {
            if (!enemy3.active)
            {
                enemy3.active = true;
                enemy3.position.x = player.position.x + GetRandomValue(2 * screenWidth, 6 * screenWidth);
                enemy3.position.y = building[(int)((enemy3.position.x + enemy3.width / 2) / building[0].size.x)].position.y - enemy3.height;
            }
        }

        if (enemy3.active)
        {
            // Updating enemy3 variables
            enemy3.head = (Rectangle){enemy3.position.x + 8, enemy3.position.y + 5, 64, 66};
            enemy3.body = (Rectangle){enemy3.position.x + 18, enemy3.position.y + 71, 45, 33};

            enemy3_active_building = (int)((enemy3.position.x + enemy3.width / 2) / building[0].size.x);

            if (player.position.x + player.width / 2 > enemy3.position.x + enemy3.width / 2)
                enemy3.direction = true;
            else
                enemy3.direction = false;

            // Updating gun variables for enenmy1
            if (enemy3.direction)
            {
                enemy3_gun.centerWrtGun.x = 42;
                enemy3_gun.centerWrtBody.x = 4;
            }
            else
            {
                enemy3_gun.centerWrtGun.x = enemy3_gun.width - 42;
                enemy3_gun.centerWrtBody.x = -51;
            }

            if (enemy3.position.x < player.position.x - screenWidth / 2)
                enemy3turn = 1;
            if (enemy3.position.x > player.position.x + screenWidth / 2)
                enemy3turn = -1;
            enemy3.position.x += enemy3turn * enemy3.speed.x * GetFrameTime();
            enemy3.position.y = building[(int)((enemy3.position.x + enemy3.width / 2) / building[0].size.x)].position.y - enemy3.height;

            // Enemy3 shoot active logic
            if (enemy3.active)
            {
                enemy3.shootTimer += GetFrameTime();

                if (enemy3.shootTimer >= 0.5f)
                {
                    enemy3.shootTimer = 0;
                    enemy3.aim = (Vector2){GetRandomValue(enemyShootingAccuracy.level1.x, enemyShootingAccuracy.level1.x + enemyShootingAccuracy.level1.width), GetRandomValue(enemyShootingAccuracy.level1.y, enemyShootingAccuracy.level1.y + enemyShootingAccuracy.level1.height)};

                    // Enemy3 gun rotation logic
                    enemy3_gun.rotationDegree = (atan2f((enemy3.aim.y - (enemy3.position.y + enemy3_gun.centerWrtBody.y + enemy3_gun.centerWrtGun.y)), (enemy3.aim.x - (enemy3.position.x + enemy3_gun.centerWrtBody.x + enemy3_gun.centerWrtGun.x))) * (180.0f / PI));
                    if (enemy3_gun.rotationDegree < 0)
                        enemy3_gun.rotationDegree += 360.0f;

                    for (int i = 0; i < ENEMY3_MAX_SHOOTS && player.position.x > enemy3.position.x - screenWidth * 2; i++)
                    {
                        if (!enemy3_shoot[i].active)
                        {
                            enemy3_shoot[i].active = true;
                            if (enemy3.direction)
                                enemy3_shoot[i].position = (Vector2){enemy3.position.x + enemy3_gun.centerWrtBody.x + enemy3_gun.centerWrtGun.x + (enemy3_gun.width - enemy3_gun.centerWrtGun.x) * cos((enemy3_gun.rotationDegree) * DEG2RAD), enemy3.position.y + enemy3_gun.centerWrtBody.y + enemy3_gun.centerWrtGun.y + (enemy3_gun.width - enemy3_gun.centerWrtGun.x) * sin((enemy3_gun.rotationDegree) * DEG2RAD)};
                            else
                                enemy3_shoot[i].position = (Vector2){enemy3.position.x + enemy3_gun.centerWrtBody.x + enemy3_gun.centerWrtGun.x - (enemy3_gun.centerWrtGun.x) * cos((enemy3_gun.rotationDegree + 180) * DEG2RAD), enemy3.position.y + enemy3_gun.centerWrtBody.y + enemy3_gun.centerWrtGun.y - (enemy3_gun.centerWrtGun.x) * sin((enemy3_gun.rotationDegree + 180) * DEG2RAD)};
                            enemy3_shoot[i].speed.x = cos(enemy3_gun.rotationDegree * DEG2RAD) * GetFrameTime() * 5600;
                            enemy3_shoot[i].speed.y = sin(enemy3_gun.rotationDegree * DEG2RAD) * GetFrameTime() * 5600;
                            enemy3_shoot[i].rotationDegree = enemy3_gun.rotationDegree;
                            break;
                        }
                    }
                }

                // Enemy3 shooting logic
                for (int i = 0; i < ENEMY3_MAX_SHOOTS; i++)
                {
                    if (enemy3_shoot[i].active)
                    {
                        if (enemy3_shoot[i].position.x > enemy3.position.x + screenWidth * 3 || enemy3_shoot[i].position.y < 0 || enemy3_shoot[i].position.x < enemy3.position.x - screenWidth * 3 || CheckCollisionRecs((Rectangle){enemy3_shoot[i].position.x, enemy3_shoot[i].position.y, enemy3_shoot[i].size.x, enemy3_shoot[i].size.y}, (Rectangle){building[enemy3_active_building].position.x, building[enemy3_active_building].position.y, building[enemy3_active_building].size.x, building[enemy3_active_building].size.y}) || CheckCollisionRecs((Rectangle){enemy3_shoot[i].position.x, enemy3_shoot[i].position.y, enemy3_shoot[i].size.x, enemy3_shoot[i].size.y}, (Rectangle){building[enemy3_active_building + 1].position.x, building[enemy3_active_building + 1].position.y, building[enemy3_active_building + 1].size.x, building[enemy3_active_building + 1].size.y}) || CheckCollisionRecs((Rectangle){enemy3_shoot[i].position.x, enemy3_shoot[i].position.y, enemy3_shoot[i].size.x, enemy3_shoot[i].size.y}, (Rectangle){building[enemy3_active_building + 2].position.x, building[enemy3_active_building + 2].position.y, building[enemy3_active_building + 2].size.x, building[enemy3_active_building + 2].size.y}) || CheckCollisionRecs((Rectangle){enemy3_shoot[i].position.x, enemy3_shoot[i].position.y, enemy3_shoot[i].size.x, enemy3_shoot[i].size.y}, (Rectangle){building[enemy3_active_building + 3].position.x, building[enemy3_active_building + 3].position.y, building[enemy3_active_building + 3].size.x, building[enemy3_active_building + 3].size.y}) || CheckCollisionRecs((Rectangle){enemy3_shoot[i].position.x, enemy3_shoot[i].position.y, enemy3_shoot[i].size.x, enemy3_shoot[i].size.y}, (Rectangle){building[enemy3_active_building - 1].position.x, building[enemy3_active_building - 1].position.y, building[enemy3_active_building - 1].size.x, building[enemy3_active_building - 1].size.y}) || CheckCollisionRecs((Rectangle){enemy3_shoot[i].position.x, enemy3_shoot[i].position.y, enemy3_shoot[i].size.x, enemy3_shoot[i].size.y}, (Rectangle){building[enemy3_active_building - 2].position.x, building[enemy3_active_building - 2].position.y, building[enemy3_active_building - 2].size.x, building[enemy3_active_building - 2].size.y}) || CheckCollisionRecs((Rectangle){enemy3_shoot[i].position.x, enemy3_shoot[i].position.y, enemy3_shoot[i].size.x, enemy3_shoot[i].size.y}, (Rectangle){building[enemy3_active_building - 3].position.x, building[enemy3_active_building - 3].position.y, building[enemy3_active_building - 3].size.x, building[enemy3_active_building - 3].size.y}))
                        {
                            enemy3_shoot[i].active = false;
                        }

                        if (CheckCollisionRecs((Rectangle){enemy3_shoot[i].position.x, enemy3_shoot[i].position.y, enemy3_shoot[i].size.x, enemy3_shoot[i].size.y}, player.head) || CheckCollisionRecs((Rectangle){enemy3_shoot[i].position.x, enemy3_shoot[i].position.y, enemy3_shoot[i].size.x, enemy3_shoot[i].size.y}, player.body))
                        {
                            enemy3_shoot[i].active = false;
                            if (sound)
                                PlaySound(sounds_gun.bullethit);

                            if (!objects.energyDrinkInUse)
                            {
                                player.health = player.health - enemy1_gun.damage;
                            }
                        }
                    }
                }
            }
        }

        // Enemy3 shoot movement update
        for (int i = 0; i < ENEMY3_MAX_SHOOTS; i++)
        {
            if (enemy3_shoot[i].active)
            {
                enemy3_shoot[i].position.x += enemy3_shoot[i].speed.x;
                enemy3_shoot[i].position.y += enemy3_shoot[i].speed.y;
            }
        }

        // Enemy 3 active logic
        if (enemy_create)
        {
            if (!enemy2.active)
            {
                enemy2.active = true;
                enemy2.position.x = player.position.x + GetRandomValue(3 * screenWidth, 7 * screenWidth);
                enemy2.position.y = building[(int)((enemy2.position.x + enemy2.width / 2) / building[0].size.x)].position.y - enemy2.height;
            }
        }

        if (enemy2.active)
        {
            // Updating enemy2 variables
            enemy2.head = (Rectangle){enemy2.position.x + 15, enemy2.position.y + 8, 65, 66};
            enemy2.body = (Rectangle){enemy2.position.x + 32, enemy2.position.y + 74, 31, 35};

            enemy2.position.x -= enemy2.speed.x * GetFrameTime();
        }
        enemy2.position.y = building[(int)((enemy2.position.x + enemy2.width / 2) / building[0].size.x)].position.y - enemy2.height;

        // Redline logic
        redline_counter += 2;
        redline_x = redline_counter * 0.1;
        if (redline_x > player.position.x)
        {
            player.health -= GetFrameTime() * 10;
        }
        if (redline_x > enemy1.position.x)
        {
            enemy1.health -= GetFrameTime() * 10;
        }
        if (redline_x > enemy2.position.x)
        {
            enemy2.health -= GetFrameTime() * 10;
        }
        if (redline_x > enemy3.position.x)
        {
            enemy3.health -= GetFrameTime() * 10;
        }

        // Object creation logic
        if (time % object_create_counter == 0)
        {
            object_create_counter += 15;
            if (GetRandomValue(0, 1) == 0)
            {
                objects.adrenalineSyringe.x = player.position.x + screenWidth;
                objects.adrenalineSyringe.y = building[(int)(objects.adrenalineSyringe.x / building[0].size.x)].position.y - objects.adrenalineSyringe.height;
                objects.adrenalineSyringeActive = true;
            }
            else
            {
                objects.energyDrink.x = player.position.x + screenWidth;
                objects.energyDrink.y = building[(int)(objects.energyDrink.x / building[0].size.x)].position.y - objects.energyDrink.height;
                objects.energyDrinkActive = true;
            }
        }

        // Object collision logic
        if (objects.adrenalineSyringeActive)
        {
            if (CheckCollisionRecs((Rectangle){player.position.x + 5, player.position.y, player.width - 10, player.height}, objects.adrenalineSyringe))
            {
                objects.adrenalineSyringeCount += 1;
                objects.adrenalineSyringeActive = false;
            }
        }
        if (objects.energyDrinkActive)
        {
            if (CheckCollisionRecs((Rectangle){player.position.x + 5, player.position.y, player.width - 10, player.height}, objects.energyDrink))
            {
                objects.energyDrinkCount += 1;
                objects.energyDrinkActive = false;
            }
        }

        // AdrenalineSyringe logic
        if (IsKeyPressed(KEY_R))
        {
            if (objects.adrenalineSyringeCount > 0)
            {
                player.health = 100;
                objects.adrenalineSyringeCount -= 1;
            }
        }

        // EnergyDrink logic
        if (IsKeyPressed(KEY_F))
        {
            if (objects.energyDrinkCount > 0)
            {
                objects.energyDrinkInUse = true;
                objects.energyDrinkTimer = GetTime();
                objects.energyDrinkCount -= 1;
            }
        }

        if (objects.energyDrinkInUse)
        {
            if (time - 1 > objects.energyDrinkTimer + 3)
            {
                objects.energyDrinkInUse = false;
            }
        }

        // Enemy out of reach deactivation logic
        if (enemy1.position.x < player.position.x - 15 * screenWidth)
        {
            enemy1.active = false;
        }
        if (enemy2.position.x < player.position.x - 15 * screenWidth)
        {
            enemy2.active = false;
        }
        if (enemy3.position.x < player.position.x - 15 * screenWidth)
        {
            enemy3.active = false;
        }

        // Level up enemies and player score logic
        if (time % levelUpCounter == 0)
        {
            levelUpCounter += 60;

            enemy1_gun.damage += 0.15f;

            enemy3.shield += 0.07f;
            enemy3.speed.x += 2;
            enemy3_gun.damage += 0.2f;

            enemy2.speed.x += 4;
            enemy2.shield += 0.07f;

            enemy1KillPoints += 100;
            enemy2KillPoints += 50;
            enemy3KillPoints += 200;
        }

        // Gameover logic
        if (player.health <= 0 || player.position.x + player.width + 100 > 1250000)
        {
            gameover = true;
        }
    }
    return 0;
}

// Draw game (one frame)
void DrawGame()
{
    if (!pause)
    {
        BeginDrawing();
        ClearBackground(RAYWHITE);
        BeginMode2D(camera);
        for (int i = 0; i < MAX_BUILDINGS; i++)
        {
            DrawRectangleV(building[i].position, building[i].size, building[i].color);
        }

        // Drawing player according to movements
        player_animation_timer += GetFrameTime();
        if (IsKeyReleased(KEY_LEFT_SHIFT))
        {
            player_animation_timer = 0;
            player_animation_Currentframe = 0;
        }
        if ((IsKeyReleased(KEY_D) || IsKeyReleased(KEY_RIGHT)) || (IsKeyReleased(KEY_A) || IsKeyReleased(KEY_LEFT)))
        {
            player_animation_timer = 0;
            player_animation_Currentframe = 0;
        }
        if (IsKeyReleased(KEY_W))
        {
            player_animation_timer = 0;
            player_animation_Currentframe = 0;
        }

        if (!player_grounded)
        {
            if (player.direction)
                DrawTextureRec(texture_player.jump.spriteSheet, (Rectangle){0, 0, player.width, player.height}, player.position, WHITE);
            else
                DrawTextureRec(texture_player.jump180.spriteSheet, (Rectangle){0, 0, player.width, player.height}, player.position, WHITE);
        }
        else if ((IsKeyDown(KEY_D) || IsKeyDown(KEY_RIGHT)) || (IsKeyDown(KEY_A) || IsKeyDown(KEY_LEFT)))
        {
            player_animation_MaxFrames = 6;
            if (player_animation_timer >= 0.08f)
            {
                player_animation_timer = 0.0f;
                player_animation_Currentframe += 1;
            }
            player_animation_Currentframe = player_animation_Currentframe % player_animation_MaxFrames;
            if (player.direction)
                DrawTextureRec(texture_player.walk.spriteSheet, (Rectangle){(player.width * player_animation_Currentframe), 0, player.width, player.height}, (Vector2){player.position.x, player.position.y + 2}, WHITE);
            else
                DrawTextureRec(texture_player.walk180.spriteSheet, (Rectangle){(player.width * player_animation_Currentframe), 0, player.width, player.height}, (Vector2){player.position.x, player.position.y + 2}, WHITE);
        }
        else if (IsKeyDown(KEY_LEFT_SHIFT))
        {
            player_animation_MaxFrames = 4;
            if (player_animation_timer >= 0.12f)
            {
                player_animation_timer = 0.0f;
                player_animation_Currentframe += 1;
            }
            player_animation_Currentframe = player_animation_Currentframe % player_animation_MaxFrames;
            if (player.direction)
                DrawTextureRec(texture_player.run.spriteSheet, (Rectangle){(player.width * player_animation_Currentframe), 0, player.width, player.height}, player.position, WHITE);
            else
                DrawTextureRec(texture_player.run180.spriteSheet, (Rectangle){(player.width * player_animation_Currentframe), 0, player.width, player.height}, player.position, WHITE);
        }
        else
        {
            player_animation_MaxFrames = 10;
            if (player_animation_timer >= 0.08f)
            {
                player_animation_timer = 0.0f;
                player_animation_Currentframe += 1;
            }
            player_animation_Currentframe = player_animation_Currentframe % player_animation_MaxFrames;
            if (player.direction)
                DrawTextureRec(texture_player.idle.spriteSheet, (Rectangle){(player.width * player_animation_Currentframe), 0, player.width, player.height}, player.position, WHITE);
            else
                DrawTextureRec(texture_player.idle180.spriteSheet, (Rectangle){(player.width * player_animation_Currentframe), 0, player.width, player.height}, player.position, WHITE);
        }

        Rectangle playerGunSoure = (Rectangle){0, 0, player_gun.width, player_gun.height};
        Rectangle playerGunDestination = (Rectangle){player.position.x + player_gun.centerWrtBody.x + player_gun.centerWrtGun.x, player.position.y + player_gun.centerWrtBody.y + player_gun.centerWrtGun.y, player_gun.width, player_gun.height};
        if (player.direction)
            DrawTexturePro(texture_weapons.gun, playerGunSoure, playerGunDestination, (Vector2){player_gun.centerWrtGun.x, player_gun.centerWrtGun.y}, player_gun.rotationDegree, WHITE);
        else
            DrawTexturePro(texture_weapons.gun180, playerGunSoure, playerGunDestination, (Vector2){player_gun.centerWrtGun.x, player_gun.centerWrtGun.y}, player_gun.rotationDegree + 180, WHITE);

        for (int i = 0; i < PLAYER_MAX_SHOOTS; i++)
        {
            if (shoot[i].active)
            {
                DrawRectanglePro((Rectangle){shoot[i].position.x, shoot[i].position.y, shoot[i].size.x, shoot[i].size.y}, (Vector2){shoot[i].size.x / 2, shoot[i].size.y / 2}, shoot[i].rotationDegree, shoot[i].color);
            }
        }

        // Player shoot aim
        if ((sqrt(pow(GetScreenToWorld2D((Vector2){screenWidth / 2, screenHeight / 2}, camera).x - mousePosition.x, 2) + pow(GetScreenToWorld2D((Vector2){screenWidth / 2, screenHeight / 2}, camera).y - mousePosition.y, 2)) < screenHeight / 2 - 10))
            DrawRectanglePro((Rectangle){((player.position.x + player_gun.centerWrtBody.x + player_gun.centerWrtGun.x) + (mousePosition.x - GetScreenToWorld2D((Vector2){screenWidth / 2, screenHeight / 2}, camera).x) - 1), ((player.position.y + player_gun.centerWrtBody.y + player_gun.centerWrtGun.y) + (mousePosition.y - GetScreenToWorld2D((Vector2){screenWidth / 2, screenHeight / 2}, camera).y) - 5), 2, 10}, (Vector2){1, 5}, player_gun.rotationDegree, RED);

        // Player health bar
        DrawRectangleLinesEx((Rectangle){GetScreenToWorld2D((Vector2){15, 10}, camera).x, GetScreenToWorld2D((Vector2){15, 10}, camera).y, 150, 20}, 1, BLACK);
        DrawRectangleV(GetScreenToWorld2D((Vector2){16, 11}, camera), (Vector2){148 * player.health / 100, 18}, PINK);
        DrawTexture(texture_symbols.heart, GetScreenToWorld2D((Vector2){5, 10}, camera).x, GetScreenToWorld2D((Vector2){0, 10}, camera).y, WHITE);

        // Player air time
        DrawRectangleLinesEx((Rectangle){GetScreenToWorld2D((Vector2){15, 40}, camera).x, GetScreenToWorld2D((Vector2){15, 40}, camera).y, 150, 20}, 1, BLACK);
        DrawRectangleV(GetScreenToWorld2D((Vector2){16, 41}, camera), (Vector2){148 * airTime / 100, 18}, YELLOW);
        DrawTexture(texture_symbols.airtime, GetScreenToWorld2D((Vector2){5, 40}, camera).x, GetScreenToWorld2D((Vector2){0, 40}, camera).y, WHITE);

        // Energydrink count
        DrawTexture(texture_symbols.enerydrink, GetScreenToWorld2D((Vector2){0, 70}, camera).x, GetScreenToWorld2D((Vector2){0, 70}, camera).y, WHITE);
        if (objects.energyDrinkInUse)
            DrawText(TextFormat("%i", objects.energyDrinkCount), GetScreenToWorld2D((Vector2){40, 77}, camera).x, GetScreenToWorld2D((Vector2){40, 77}, camera).y, 20, RED);
        else
            DrawText(TextFormat("%i", objects.energyDrinkCount), GetScreenToWorld2D((Vector2){40, 77}, camera).x, GetScreenToWorld2D((Vector2){40, 77}, camera).y, 20, BLACK);

        // Adrenaline syringe count
        DrawTexture(texture_symbols.adrenalinesyringe, GetScreenToWorld2D((Vector2){0, 110}, camera).x, GetScreenToWorld2D((Vector2){0, 110}, camera).y, WHITE);
        DrawText(TextFormat("%i", objects.adrenalineSyringeCount), GetScreenToWorld2D((Vector2){40, 117}, camera).x, GetScreenToWorld2D((Vector2){40, 117}, camera).y, 20, BLACK);

        // Redline drawing logic
        DrawRectangleV((Vector2){0, 0}, (Vector2){redline_x, screenHeight}, Fade(RED, 0.2f));

        // Enemy1 drawing
        if (enemy1.active)
        {
            enemy1_animation_timer += GetFrameTime();
            enemy1_animation_MaxFrames = 10;
            if (enemy1_animation_timer >= 0.08f)
            {
                enemy1_animation_timer = 0.0f;
                enemy1_animation_Currentframe += 1;
            }
            enemy1_animation_Currentframe = enemy1_animation_Currentframe % enemy1_animation_MaxFrames;
            if (enemy1.direction)
                DrawTextureRec(texture_enemy1.idle.spriteSheet, (Rectangle){(enemy1.width * enemy1_animation_Currentframe), 0, enemy1.width, enemy1.height}, enemy1.position, WHITE);
            else
                DrawTextureRec(texture_enemy1.idle180.spriteSheet, (Rectangle){(enemy1.width * enemy1_animation_Currentframe), 0, enemy1.width, enemy1.height}, enemy1.position, WHITE);

            // Enemy1 gun
            Rectangle enemy1GunSoure = (Rectangle){0, 0, enemy1_gun.width, enemy1_gun.height};
            Rectangle enemy1GunDestination = (Rectangle){enemy1.position.x + enemy1_gun.centerWrtBody.x + enemy1_gun.centerWrtGun.x, enemy1.position.y + enemy1_gun.centerWrtBody.y + enemy1_gun.centerWrtGun.y, enemy1_gun.width, enemy1_gun.height};
            if (enemy1.direction)
                DrawTexturePro(texture_weapons.gun, enemy1GunSoure, enemy1GunDestination, (Vector2){enemy1_gun.centerWrtGun.x, enemy1_gun.centerWrtGun.y}, enemy1_gun.rotationDegree, WHITE);
            else
                DrawTexturePro(texture_weapons.gun180, enemy1GunSoure, enemy1GunDestination, (Vector2){enemy1_gun.centerWrtGun.x, enemy1_gun.centerWrtGun.y}, enemy1_gun.rotationDegree + 180, WHITE);
        }

        // Enemy1 shoot
        for (int i = 0; i < ENEMY1_MAX_SHOOTS; i++)
        {
            if (enemy1_shoot[i].active)
            {
                DrawRectanglePro((Rectangle){enemy1_shoot[i].position.x, enemy1_shoot[i].position.y, enemy1_shoot[i].size.x, enemy1_shoot[i].size.y}, (Vector2){enemy1_shoot[i].size.x / 2, enemy1_shoot[i].size.y / 2}, enemy1_shoot[i].rotationDegree, enemy1_shoot[i].color);
            }
        }

        if (enemy3.active)
        {
            enemy3_animation_timer += GetFrameTime();
            enemy3_animation_MaxFrames = 6;
            if (enemy3_animation_timer >= 0.08f)
            {
                enemy3_animation_timer = 0.0f;
                enemy3_animation_Currentframe += 1;
            }
            enemy3_animation_Currentframe = enemy3_animation_Currentframe % enemy3_animation_MaxFrames;
            if (enemy3.direction)
                DrawTextureRec(texture_enemy3.walk.spriteSheet, (Rectangle){(enemy3.width * enemy3_animation_Currentframe), 0, enemy3.width, enemy3.height}, enemy3.position, WHITE);
            else
                DrawTextureRec(texture_enemy3.walk180.spriteSheet, (Rectangle){(enemy3.width * enemy3_animation_Currentframe), 0, enemy3.width, enemy3.height}, enemy3.position, WHITE);
            // Enemy3 gun
            Rectangle enemy3GunSoure = (Rectangle){0, 0, enemy3_gun.width, enemy3_gun.height};
            Rectangle enemy3GunDestination = (Rectangle){enemy3.position.x + enemy3_gun.centerWrtBody.x + enemy3_gun.centerWrtGun.x, enemy3.position.y + enemy3_gun.centerWrtBody.y + enemy3_gun.centerWrtGun.y, enemy3_gun.width, enemy3_gun.height};
            if (enemy3.direction)
                DrawTexturePro(texture_weapons.gun, enemy3GunSoure, enemy3GunDestination, (Vector2){enemy3_gun.centerWrtGun.x, enemy3_gun.centerWrtGun.y}, enemy3_gun.rotationDegree, WHITE);
            else
                DrawTexturePro(texture_weapons.gun180, enemy3GunSoure, enemy3GunDestination, (Vector2){enemy3_gun.centerWrtGun.x, enemy3_gun.centerWrtGun.y}, enemy3_gun.rotationDegree + 180, WHITE);
        }

        // Enemy3 shoot
        for (int i = 0; i < ENEMY3_MAX_SHOOTS; i++)
        {
            if (enemy3_shoot[i].active)
            {
                DrawRectanglePro((Rectangle){enemy3_shoot[i].position.x, enemy3_shoot[i].position.y, enemy3_shoot[i].size.x, enemy3_shoot[i].size.y}, (Vector2){enemy3_shoot[i].size.x / 2, enemy3_shoot[i].size.y / 2}, enemy3_shoot[i].rotationDegree, enemy3_shoot[i].color);
            }
        }

        if (enemy2.active)
        {
            enemy2_animation_timer += GetFrameTime();
            enemy2_animation_MaxFrames = 6;
            if (enemy2_animation_timer >= 0.08f)
            {
                enemy2_animation_timer = 0.0f;
                enemy2_animation_Currentframe += 1;
            }
            enemy2_animation_Currentframe = enemy2_animation_Currentframe % enemy2_animation_MaxFrames;
            DrawTextureRec(texture_enemy2.run.spriteSheet, (Rectangle){(enemy2.width * enemy2_animation_Currentframe), 0, enemy2.width, enemy2.height}, enemy2.position, WHITE);
        }
        sprintf(scorestring, "%d", score);
        DrawText(TextFormat("%i", score), GetScreenToWorld2D((Vector2){(GetScreenWidth() - MeasureText(scorestring, 30) - 10), 10}, camera).x, 10, 30, BLACK);

        // Objects draw
        if (objects.adrenalineSyringeActive)
        {
            DrawTexture(texture_objects.adrenalineSyringe, objects.adrenalineSyringe.x, objects.adrenalineSyringe.y, WHITE);
        }
        if (objects.energyDrinkActive)
        {
            DrawTexture(texture_objects.energyDrink, objects.energyDrink.x, objects.energyDrink.y, WHITE);
        }
        EndMode2D();
        EndDrawing();
    }
}

int Init()
{
    for (int i = 0; i < MAX_BUILDINGS; i++)
    {
        int randomValue;
        while (true)
        {
            if (i == 0)
            {
                randomValue = GetRandomValue(50, 250);
                break;
            }
            else
            {
                randomValue = GetRandomValue(50, 250);
                if ((randomValue >= building[i - 1].size.y + 20 && randomValue <= building[i - 1].size.y + 70) || (randomValue <= building[i - 1].size.y - 20 && randomValue >= building[i - 1].size.y - 70))
                {
                    break;
                }
            }
        }
        building[i].size = (Vector2){500, randomValue};
        building[i].position = (Vector2){building[i].size.x * i, screenHeight - building[i].size.y};
        building[i].color = BLACK;
    }

    player.width = 80;
    player.height = 104;
    player.position = (Vector2){750 - player.width / 2, screenHeight - building[1].size.y - player.height};
    player.direction = true;
    player.speed.x = 600.0f;
    player.speed.y = 1300.0f;
    player.head = (Rectangle){player.position.x + 8, player.position.y + 11, 64, 60};
    player.body = (Rectangle){player.position.x + 18, player.position.y + 71, 45, 33};
    player.health = 100.0f;

    camera.target = (Vector2){screenWidth / 2.0f, screenHeight / 2.0f};
    camera.offset = (Vector2){screenWidth / 2.0f, screenHeight / 2.0f};
    camera.rotation = 0.0f;
    camera.zoom = 1.0f;

    player_gun.width = 127;
    player_gun.height = 30;
    player_gun.centerWrtGun.x = 42;
    player_gun.centerWrtGun.y = 16;
    player_gun.centerWrtBody.x = 4;
    player_gun.centerWrtBody.y = 69;
    player_gun.damage = 30;

    score = 0;
    gameover = false;
    quit = false;
    pause = false;
    redline_counter = 0;
    sound = true;
    exitGame = false;

    for (int i = 0; i < PLAYER_MAX_SHOOTS; i++)
    {
        shoot[i].position = (Vector2){0, 0};
        shoot[i].speed = (Vector2){0, 0};
        shoot[i].size = (Vector2){15, 5};
        shoot[i].active = false;
        shoot[i].color = ORANGE;
    }

    enemy1.position = (Vector2){0, 0};
    enemy1.width = 80;
    enemy1.height = 104;
    enemy1.direction = false;
    enemy1.head = (Rectangle){enemy1.position.x + 8, enemy1.position.y + 11, 64, 60};
    enemy1.body = (Rectangle){enemy1.position.x + 18, enemy1.position.y + 71, 45, 33};
    enemy1.health = 100.0f;
    enemy1.shield = 0.0f;
    enemy1.active = false;

    enemy1_gun.width = 127;
    enemy1_gun.height = 30;
    enemy1_gun.centerWrtGun.x = enemy1_gun.width - 42;
    enemy1_gun.centerWrtGun.y = 16;
    enemy1_gun.centerWrtBody.x = -51;
    enemy1_gun.centerWrtBody.y = 69;
    enemy1_gun.damage = 10.0f;

    enemyShootingAccuracy.level1 = (Rectangle){player.position.x, player.position.y, player.width, player.height};
    enemyShootingAccuracy.level2 = (Rectangle){player.position.x, player.position.y + 1, player.width, 90};
    enemyShootingAccuracy.level2 = (Rectangle){player.position.x + 4, player.position.y + 6, 71, 61};

    for (int i = 0; i < ENEMY1_MAX_SHOOTS; i++)
    {
        enemy1_shoot[i].position = (Vector2){0, 0};
        enemy1_shoot[i].speed = (Vector2){0, 0};
        enemy1_shoot[i].size = (Vector2){15, 5};
        enemy1_shoot[i].active = false;
        enemy1_shoot[i].color = ORANGE;
    }

    enemy3.position = (Vector2){0, 0};
    enemy3.width = 80;
    enemy3.height = 104;
    enemy3.speed.x = 500;
    enemy3.head = (Rectangle){enemy3.position.x + 8, enemy3.position.y + 5, 64, 66};
    enemy3.body = (Rectangle){enemy3.position.x + 18, enemy3.position.y + 71, 45, 33};
    enemy3.health = 100.0f;
    enemy3.shield = 7.0f;
    enemy3.active = false;

    enemy3_gun.width = 127;
    enemy3_gun.height = 30;
    enemy3_gun.centerWrtGun.x = enemy3_gun.width - 42;
    enemy3_gun.centerWrtGun.y = 16;
    enemy3_gun.centerWrtBody.x = -51;
    enemy3_gun.centerWrtBody.y = 69;
    enemy3_gun.damage = 15.0f;

    for (int i = 0; i < ENEMY3_MAX_SHOOTS; i++)
    {
        enemy3_shoot[i].position = (Vector2){0, 0};
        enemy3_shoot[i].speed = (Vector2){0, 0};
        enemy3_shoot[i].size = (Vector2){15, 5};
        enemy3_shoot[i].active = false;
        enemy3_shoot[i].color = ORANGE;
    }

    enemy2.position = (Vector2){0, 0};
    enemy2.width = 103;
    enemy2.height = 110;
    enemy2.speed.x = 450;
    enemy2.head = (Rectangle){enemy2.position.x + 15, enemy2.position.y + 8, 65, 66};
    enemy2.body = (Rectangle){enemy2.position.x + 32, enemy2.position.y + 74, 31, 35};
    enemy2.health = 100.0f;
    enemy2.shield = 3.0f;
    enemy2.active = false;

    objects.adrenalineSyringe.width = 11;
    objects.adrenalineSyringe.height = 45;
    objects.adrenalineSyringeActive = false;
    objects.adrenalineSyringeCount = 0;
    objects.energyDrink.width = 22;
    objects.energyDrink.height = 40;
    objects.energyDrinkActive = false;
    objects.energyDrinkCount = 0;

    enemy1KillPoints = 500;
    enemy2KillPoints = 300;
    enemy3KillPoints = 1000;

    return 0;
}

int LoadGameTextures()
{

    texture_player.run.spriteSheet = LoadTexture("resources/player/run/run.png");
    texture_player.run180.spriteSheet = LoadTexture("resources/player/run/run180.png");
    texture_player.idle.spriteSheet = LoadTexture("resources/player/idle/idle.png");
    texture_player.idle180.spriteSheet = LoadTexture("resources/player/idle/idle180.png");
    texture_player.walk.spriteSheet = LoadTexture("resources/player/walk/walk.png");
    texture_player.walk180.spriteSheet = LoadTexture("resources/player/walk/walk180.png");
    texture_player.jump.spriteSheet = LoadTexture("resources/player/jump/jump.png");
    texture_player.jump180.spriteSheet = LoadTexture("resources/player/jump/jump180.png");

    texture_weapons.gun = LoadTexture("resources/weapons/gun1.png");
    texture_weapons.gun180 = LoadTexture("resources/weapons/gun1180.png");

    texture_enemy1.idle.spriteSheet = LoadTexture("resources/enemy/enemy1/idle/idle.png");
    texture_enemy1.idle180.spriteSheet = LoadTexture("resources/enemy/enemy1/idle/idle180.png");

    texture_enemy2.run.spriteSheet = LoadTexture("resources/enemy/enemy2/run/run.png");

    texture_enemy3.walk.spriteSheet = LoadTexture("resources/enemy/enemy3/walk/walk.png");
    texture_enemy3.walk180.spriteSheet = LoadTexture("resources/enemy/enemy3/walk/walk180.png");

    texture_objects.adrenalineSyringe = LoadTexture("resources/player/health/adrenalinesyringe.png");
    texture_objects.energyDrink = LoadTexture("resources/player/health/energydrink.png");

    texture_symbols.heart = LoadTexture("resources/symbols/heart.png");
    texture_symbols.airtime = LoadTexture("resources/symbols/airtime.png");
    texture_symbols.enerydrink = LoadTexture("resources/symbols/energydrink.png");
    texture_symbols.adrenalinesyringe = LoadTexture("resources/symbols/adrenalinesyringe.png");

    texture_others.mainscreen = LoadTexture("resources/others/mainscreen.png");
    texture_others.menuscreen = LoadTexture("resources/others/menuscreen/menuscreen.png");
    texture_others.playButton = LoadTexture("resources/others/menuscreen/playbutton.png");
    texture_others.highestscoreButton = LoadTexture("resources/others/menuscreen/highestscorebutton.png");
    texture_others.helpButton = LoadTexture("resources/others/menuscreen/helpbutton.png");
    texture_others.exitButton = LoadTexture("resources/others/menuscreen/exitbutton.png");
    texture_others.helpscreen = LoadTexture("resources/others/helpscreen.png");
    texture_others.gameoverscreen = LoadTexture("resources/others/gameoverscreen.png");
    texture_others.resumeButton = LoadTexture("resources/others/pausescreen/resumebutton.png");
    texture_others.quitButton = LoadTexture("resources/others/pausescreen/quitbutton.png");
    texture_others.soundOnButton = LoadTexture("resources/others/pausescreen/soundOnbutton.png");
    texture_others.soundOffButton = LoadTexture("resources/others/pausescreen/soundOffbutton.png");

    return 0;
};

int UnloadGameTextures()
{
    UnloadTexture(texture_player.run.spriteSheet);
    UnloadTexture(texture_player.run180.spriteSheet);
    UnloadTexture(texture_player.idle.spriteSheet);
    UnloadTexture(texture_player.idle180.spriteSheet);
    UnloadTexture(texture_player.walk.spriteSheet);
    UnloadTexture(texture_player.walk180.spriteSheet);
    UnloadTexture(texture_player.jump.spriteSheet);
    UnloadTexture(texture_player.jump180.spriteSheet);

    UnloadTexture(texture_weapons.gun);
    UnloadTexture(texture_weapons.gun180);

    UnloadTexture(texture_enemy1.idle.spriteSheet);
    UnloadTexture(texture_enemy1.idle180.spriteSheet);

    UnloadTexture(texture_enemy2.run.spriteSheet);

    UnloadTexture(texture_enemy3.walk.spriteSheet);
    UnloadTexture(texture_enemy3.walk180.spriteSheet);

    UnloadTexture(texture_objects.adrenalineSyringe);
    UnloadTexture(texture_objects.energyDrink);

    UnloadTexture(texture_symbols.heart);
    UnloadTexture(texture_symbols.airtime);
    UnloadTexture(texture_symbols.enerydrink);
    UnloadTexture(texture_symbols.adrenalinesyringe);

    UnloadTexture(texture_others.mainscreen);
    UnloadTexture(texture_others.menuscreen);
    UnloadTexture(texture_others.playButton);
    UnloadTexture(texture_others.highestscoreButton);
    UnloadTexture(texture_others.helpButton);
    UnloadTexture(texture_others.exitButton);
    UnloadTexture(texture_others.resumeButton);
    UnloadTexture(texture_others.quitButton);
    UnloadTexture(texture_others.soundOnButton);
    UnloadTexture(texture_others.soundOffButton);
    UnloadTexture(texture_others.helpscreen);
    UnloadTexture(texture_others.gameoverscreen);
    return 0;
}

// int gamesound()
// {
//     return 0;
// }

int LoadGameSounds()
{
    sounds_gun.sound1 = LoadSound("resources/sounds/gun/sound1.wav");
    SetSoundVolume(sounds_gun.sound1, 0.3f);
    sounds_gun.bullethit = LoadSound("resources/sounds/gun/bullethit.mp3");
    SetSoundVolume(sounds_gun.bullethit, 0.7f);
    return 0;
}

int UnloadGameSounds()
{
    UnloadSound(sounds_gun.sound1);
    UnloadSound(sounds_gun.bullethit);
    return 0;
}

int MainScreen()
{
    while (!IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
    {
        BeginDrawing();
        ClearBackground(BLACK);
        DrawRectangleGradientV(0, 0, screenWidth, screenHeight, Fade(RAYWHITE, 0.990f), Fade(BLACK, 0.1f));
        DrawTexture(texture_others.mainscreen, -1, -1, WHITE);
        EndDrawing();

        if (WindowShouldClose() || IsKeyPressed(KEY_ESCAPE))
        {
            exitGame = true;
            break;
        }
    }
    return 0;
}

int MenuScreen()
{
    // Fetching data from data.txt
    FetchData();

    Button playButton = {(Vector2){470, 350}, (Vector2){113, 60}, false, false};
    Button highestscoreButton = {(Vector2){468, 425}, (Vector2){239, 60}, false, false};
    Button helpButton = {(Vector2){471, 500}, (Vector2){113, 60}, false, false};
    Button exitButton = {(Vector2){468, 575}, (Vector2){113, 60}, false, false};
    
    // Application base loop
    while (!WindowShouldClose())
    {
        BeginDrawing();
        ClearBackground(BLACK);
        DrawRectangleGradientV(0, 0, screenWidth, screenHeight, Fade(RAYWHITE, 0.990f), Fade(BLACK, 0.1f));
        DrawTexture(texture_others.menuscreen, -1, -1, WHITE);
        DrawTextureV(texture_others.playButton, playButton.position, WHITE);
        DrawTextureV(texture_others.highestscoreButton, highestscoreButton.position, WHITE);
        DrawTextureV(texture_others.helpButton, helpButton.position, WHITE);
        DrawTextureV(texture_others.exitButton, exitButton.position, WHITE);

        if (ButtonHover(playButton.position, playButton.size, GetMousePosition()))
        {
            DrawRectangleRec((Rectangle){playButton.position.x, playButton.position.y, playButton.size.x, playButton.size.y}, Fade(WHITE, 0.05f));
        }
        if (ButtonHover(highestscoreButton.position, highestscoreButton.size, GetMousePosition()))
        {
            DrawRectangleRec((Rectangle){highestscoreButton.position.x, highestscoreButton.position.y, highestscoreButton.size.x, highestscoreButton.size.y}, Fade(WHITE, 0.05f));
        }
        if (ButtonHover(helpButton.position, helpButton.size, GetMousePosition()))
        {
            DrawRectangleRec((Rectangle){helpButton.position.x, helpButton.position.y, helpButton.size.x, helpButton.size.y}, Fade(WHITE, 0.05f));
        }
        if (ButtonHover(exitButton.position, exitButton.size, GetMousePosition()))
        {
            DrawRectangleRec((Rectangle){exitButton.position.x, exitButton.position.y, exitButton.size.x, exitButton.size.y}, Fade(WHITE, 0.05f));
        }
        EndDrawing();

        if (ButtonClicked(playButton.position, playButton.size, GetMousePosition()))
        {
            Init();
            HideCursor();
            Game();
            ShowCursor();
            if (gameover)
            {
                GameOverScreen();
            }
        }
        if (ButtonClicked(highestscoreButton.position, highestscoreButton.size, GetMousePosition()))
        {
            HighestScoreScreen();
        }
        if (ButtonClicked(helpButton.position, helpButton.size, GetMousePosition()))
        {
            HelpScreen();
        }
        if (ButtonClicked(exitButton.position, exitButton.size, GetMousePosition()))
        {
            exitGame = true;
        }

        if (IsKeyPressed(KEY_ESCAPE) || exitGame)
        {
            break;
        }
    }
    return 0;
}

int HelpScreen()
{
    while (!IsKeyReleased(KEY_ESCAPE))
    {
        BeginDrawing();
        ClearBackground(BLACK);
        DrawRectangleGradientV(0, 0, screenWidth, screenHeight, Fade(RAYWHITE, 0.990f), Fade(BLACK, 0.8f));
        DrawTexture(texture_others.helpscreen, -1, -1, WHITE);
        EndDrawing();

        if (WindowShouldClose())
        {
            exitGame = true;
            break;
        }
    }
    return 0;
}

int HighestScoreScreen()
{
    sprintf(scorestring, "%d", highestscore);
    while (!IsKeyReleased(KEY_ESCAPE))
    {
        BeginDrawing();
        ClearBackground(BLACK);
        DrawRectangleGradientV(0, 0, screenWidth, screenHeight, Fade(RAYWHITE, 0.990f), Fade(BLACK, 0.8f));
        DrawText(TextFormat("%d", highestscore), (screenWidth / 2 - MeasureText(scorestring, 58) / 2), (screenHeight / 2), 58, BLACK);
        EndDrawing();

        if (WindowShouldClose())
        {
            exitGame = true;
            break;
        }
    }
    return 0;
}

int GameOverScreen()
{
    while (!IsKeyReleased(KEY_ESCAPE))
    {
        BeginDrawing();
        ClearBackground(BLACK);
        DrawRectangleGradientV(0, 0, screenWidth, screenHeight, Fade(RAYWHITE, 0.990f), Fade(BLACK, 0.8f));
        DrawTexture(texture_others.gameoverscreen, -1, -1, WHITE);
        DrawText(TextFormat("%i", score), (GetScreenWidth() / 2 - MeasureText(scorestring, 58) / 2 + 3), 490, 58, BLACK);
        EndDrawing();
        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
        {
            break;
        }
        if (WindowShouldClose())
        {
            exitGame = true;
            break;
        }
    }

    if (score > highestscore)
    {
        FeedData();
        highestscore = score;
    }
    return 0;
}

int PauseScreen()
{
    Button resumeButton = {(Vector2){470, 350}, (Vector2){137, 60}, false, false};
    Button soundButton = {(Vector2){471, 425}, (Vector2){137, 60}, false, false};
    Button quitButton = {(Vector2){468, 500}, (Vector2){113, 60}, false, false};

    while (true)
    {
        BeginDrawing();
        ClearBackground(BLACK);
        DrawRectangleGradientV(0, 0, screenWidth, screenHeight, Fade(RAYWHITE, 0.990f), Fade(BLACK, 0.1f));
        DrawTextureV(texture_others.resumeButton, resumeButton.position, WHITE);
        if (sound)
        {
            DrawTextureV(texture_others.soundOnButton, soundButton.position, WHITE);
        }
        else
        {
            DrawTextureV(texture_others.soundOffButton, soundButton.position, WHITE);
        }
        DrawTextureV(texture_others.quitButton, quitButton.position, WHITE);

        if (ButtonHover(resumeButton.position, resumeButton.size, GetMousePosition()))
        {
            DrawRectangleRec((Rectangle){resumeButton.position.x, resumeButton.position.y, resumeButton.size.x, resumeButton.size.y}, Fade(WHITE, 0.05f));
        }
        if (ButtonHover(soundButton.position, soundButton.size, GetMousePosition()))
        {
            DrawRectangleRec((Rectangle){soundButton.position.x, soundButton.position.y, soundButton.size.x, soundButton.size.y}, Fade(WHITE, 0.05f));
        }
        if (ButtonHover(quitButton.position, quitButton.size, GetMousePosition()))
        {
            DrawRectangleRec((Rectangle){quitButton.position.x, quitButton.position.y, quitButton.size.x, quitButton.size.y}, Fade(WHITE, 0.05f));
        }
        EndDrawing();

        if (ButtonClicked(resumeButton.position, resumeButton.size, GetMousePosition()) || IsKeyPressed(KEY_ESCAPE))
        {
            pause = false;
            HideCursor();
            break;
        }
        if (ButtonHover(soundButton.position, soundButton.size, GetMousePosition()) && IsMouseButtonReleased(MOUSE_BUTTON_LEFT))
        {
            sound = !sound;
        }
        if (ButtonHover(quitButton.position, quitButton.size, GetMousePosition()) && IsMouseButtonReleased(MOUSE_BUTTON_LEFT))
        {
            quit = true;
            break;
        }
        if (WindowShouldClose())
        {
            exitGame = true;
            break;
        }
    }
    return 0;
}

int FetchData()
{
    decrypt("data.txt");
    filepointer = fopen("data.txt", "r");
    fgets(scorestring, 10, filepointer);
    highestscore = atoi(scorestring);
    fclose(filepointer);
    encrypt("data.txt");
    return 0;
}

int FeedData()
{
    decrypt("data.txt");
    filepointer = fopen("data.txt", "w");
    fputs(scorestring, filepointer);
    fclose(filepointer);
    encrypt("data.txt");
    return 0;
}