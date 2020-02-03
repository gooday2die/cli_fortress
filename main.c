#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <unistd.h>
#include <stdbool.h>

#define GRAVITY_CONSTANT 9.8
#define WIDTH 200
#define HEIGHT 50
#define GROUND_HEIGHT 10
#define BULLET_SCREEN_DELAY 5000
#define TANK_SCREEN_DELAY 30000
#define HITRANGE 2

#define KNRM  "\x1B[0m"
#define KRED  "\x1B[31m"
#define KGRN  "\x1B[32m"
#define KYEL  "\x1B[33m"
#define KBLU  "\x1B[34m"
#define KMAG  "\x1B[35m"
#define KCYN  "\x1B[36m"
#define KWHT  "\x1B[37m"

typedef struct bullet_data_values{
    int init_y;
    int init_x;
    int angle;
    int velocity;
} BULLETDATA;

typedef struct _tank{
    char health; // Three health. One for one shot.
    int x;
    int y;
    char color;
} TANK;


int indexer(int, int);
void draw_screen(char*);
void make_screen_matrix(char**);
void set_pixel_color(char);
void set_map(char*);
int bullet_trajectory_on_x(BULLETDATA, int);
int draw_trajectory(BULLETDATA, char*, bool);
void refresh_screen(char*, bool);
void set_tank(TANK, char*);
void move_tank(char, TANK*, char*);
void init_game(void);
char game_system(TANK*, char*);
void clear_tank(char*, TANK);
BULLETDATA ask_shoot_command(TANK);
void ask_move_command(TANK*, char*);
int shoot(TANK, char*);
void hit_detection(TANK*, int);
void ask_command(TANK*, TANK*, char*);
bool is_valid_move_command(TANK, char*, int);


void hit_detection(TANK* tank_array, int bullet_ground_hit_x){
    if(tank_array[0].x >= bullet_ground_hit_x - HITRANGE && tank_array[0].x <= bullet_ground_hit_x + HITRANGE){
        tank_array[0].health --;
        printf("Tank 1 was hit !\n");
    }
    else if(tank_array[1].x >= bullet_ground_hit_x - HITRANGE && tank_array[1].x <= bullet_ground_hit_x + HITRANGE){
        printf("Tank 2 was hit !\n");
        tank_array[1].health --;
    }
    else {
        printf("Nobody was hit!\n");
    }
    
    printf("Remaining Health of Tank 1 : %d\n" , tank_array[0].health);
    printf("Remaining Health of Tank 2 : %d\n" , tank_array[1].health);

    return;
}

/// ETC : 10 , 44 hits tank 2.
/*
int shoot(TANK tank, char* matrix){
    BULLETDATA temp_bullet_data;
    temp_bullet_data = ask_shoot_command(tank);
    
    draw_trajectory(temp_bullet_data, matrix);
    
}
*/
void ask_move_command(TANK* tank, char* matrix){
    int command = 0;
    do {
        printf("===== Move Command for Tank %d =====\n", tank->color - 6);
        printf("Move Amount (Less than abs of 5): \n");
        printf("+ : Right / - : Left\n");
        scanf("%d", &command);
        printf("tc %d " , tank->x);

    }while(!is_valid_move_command(*tank, matrix, command));
    
    move_tank((char)command, tank, matrix);
    printf("Moved Tank %d Successfully!\n", tank->color - 6);
    return;
}

bool is_valid_move_command(TANK tank, char* matrix, int command){
    int temp_tank_coordinate = 0;
    temp_tank_coordinate = tank.x + command * 1;
    printf("ttc %d\n", temp_tank_coordinate);
    
    
    if (abs(command) <= 5){
        if (temp_tank_coordinate + 5 >= WIDTH - 1 || temp_tank_coordinate - 5 <= 0) {
            printf("You cannot move over there! Its out of map!\n");
            return false; // When Tank exceeds the boundary of the map.
        }
        else return true;
    }
    else return false;
}

void ask_command(TANK* tank_array, TANK* tank, char* matrix){
    int command = 0;
    do{
        printf("===== Command for Tank %d =====\n", tank->color - 6);
        printf("Remaining Health of Tank 1 : %d\n" , tank_array[0].health);
        printf("Remaining Health of Tank 2 : %d\n" , tank_array[1].health);
        printf("1. Move Tank\n");
        printf("2. Shoot\n");
        printf("3. Pass this turn\n");
        scanf("%d", &command);
    }while(!((command >= 1) && (command <= 3)));
    
    switch(command){
        case 1:
        {
            ask_move_command(tank, matrix);
            break;
        }
        case 2:
        {
            BULLETDATA temp_bullet;
            int ground_hit_x = 0;

            temp_bullet = ask_shoot_command(*tank);
            ground_hit_x = draw_trajectory(temp_bullet, matrix, tank->color - 7); // 0 is not reversed, 1 is reversed.
            hit_detection(tank_array, ground_hit_x);
            
            set_tank(tank_array[0], matrix);
            set_tank(tank_array[1], matrix);
            refresh_screen(matrix, 1);
            // to avoid that, I added 3 more lines and recreate the tank.
            
        }
        default:
            break;
    }
    return;
}

BULLETDATA ask_shoot_command(TANK tank){
    BULLETDATA temp_command;
    
    printf("===== Shoot Command for Tank %d =====\n", tank.color - 6);
    printf("Angle : ");
    scanf("%d", &temp_command.angle);
    printf("Power : ");
    scanf("%d", &temp_command.velocity);
    
    temp_command.init_x = tank.x;
    temp_command.init_y = tank.y;
    
    return temp_command;
}

char game_system(TANK* tank_array, char* matrix){
    char winner = 0;
    while (tank_array[0].health != 0 || tank_array[1].health != 0){
        
        ask_command(tank_array, tank_array, matrix);
        if (tank_array[0].health == 0 || tank_array[1].health == 0) break;
        ask_command(tank_array, tank_array + 1, matrix);
    }
    
    if (tank_array[0].health == 0) winner = 2;
    else winner = 1;
    
    return winner;
}

void init_game(void){ // returns winner id
    char* matrix = NULL;
    char winner = 0;
    TANK tank_array[2];
    
    make_screen_matrix(&matrix);
    set_map(matrix);
    
    tank_array[0].health = 3;
    tank_array[0].x = 10;
    tank_array[0].y = 10;
    tank_array[0].color = 7;
    
    tank_array[1].health = 3;
    tank_array[1].x = 189;
    tank_array[1].y = 10;
    tank_array[1].color = 8;
    
    set_tank(tank_array[0], matrix);
    set_tank(tank_array[1], matrix);
    draw_screen(matrix);
    
    winner = game_system(tank_array, matrix);
    printf("Tank %d won the game\n" , winner);
    
    return;
}


void clear_tank(char* matrix, TANK tank){
    for (int i = tank.x - 2 ; i < tank.x + 3 ; i++) matrix[indexer(tank.y, i)] = 0;
    for (int i = tank.x - 1 ; i < tank.x + 2 ; i++) matrix[indexer(tank.y + 1, i)] = 0;
    
    return;
}


void move_tank(char amount, TANK* tank, char* matrix){
    for (int i = 0 ; i < abs(amount) * 2 ; i++){ // Since the tank can go backwards.
        clear_tank(matrix, *tank);
        
        if (amount > 0) tank->x ++;
        else tank->x --;
        
        set_tank(*tank, matrix);
        refresh_screen(matrix, 0);
    }
}

void set_tank(TANK tank, char* matrix){
    /*
     Tank Looks like this
      ###
     #####
     ^
     this is the center point of tank.
     */
    
    
    // Tank 1 color is Blue and code is 7
    // Tank 2 color is Magenta and code is 8
    for(int i = tank.x - 2; i < tank.x + 3; i++) matrix[indexer(tank.y,i)] = tank.color;
    for(int i = tank.x - 1; i < tank.x + 2; i++) matrix[indexer(tank.y+1, i)] = tank.color;
}

void refresh_screen(char* matrix, bool type){
    // type true => bullet, type false => tank
    system("clear");
    draw_screen(matrix);
    if (type) usleep(BULLET_SCREEN_DELAY);
    else usleep(TANK_SCREEN_DELAY);
    
}

int draw_trajectory(BULLETDATA data, char* matrix, bool reverse){
    // if reversed -> right to left
    // if not reversed -> left to right
    int i = data.init_x;
    if (!reverse){
        for (; i < WIDTH ; i++){
            int temp_y = 0;
            temp_y = bullet_trajectory_on_x(data, i);
            if ((GROUND_HEIGHT <= temp_y) && (temp_y <= HEIGHT)) matrix[indexer(temp_y, i)] = 2; // When the screen cannot represent trajectory
            else if (temp_y < GROUND_HEIGHT) break; // When the bullet hits ground
            // printf("%d\n", temp_y); // for debug
            refresh_screen(matrix, 1);
        }

        for (int i = data.init_x + 2 ; i < WIDTH ; i++){
            int temp_y = 0;
            temp_y = bullet_trajectory_on_x(data, i);
            if ((GROUND_HEIGHT <= temp_y) && (temp_y <= HEIGHT)) matrix[indexer(temp_y, i)] = 0; // When the screen cannot represent trajectory
            else if (temp_y < GROUND_HEIGHT) break; // When the bullet hits ground
            // printf("%d\n", temp_y); // for debug
            refresh_screen(matrix, 1);
        }
    }
    else{ // if reversed, the angle must be fliped WRT y axis.
        for (; i > 0 ; i--){
            int temp_y = 0;
            temp_y = bullet_trajectory_on_x(data, i);
            if ((GROUND_HEIGHT <= temp_y) && (temp_y <= HEIGHT)) matrix[indexer(temp_y, i)] = 2; // When the screen cannot represent trajectory
            else if (temp_y < GROUND_HEIGHT) break; // When the bullet hits ground
            // printf("%d\n", temp_y); // for debug
            refresh_screen(matrix, 1);
        }
        
        for (int i = data.init_x; i > 0 ; i--){
            int temp_y = 0;
            temp_y = bullet_trajectory_on_x(data, i);
            if ((GROUND_HEIGHT <= temp_y) && (temp_y <= HEIGHT)) matrix[indexer(temp_y, i)] = 0; // When the screen cannot represent trajectory
            else if (temp_y < GROUND_HEIGHT) break; // When the bullet hits ground
            // printf("%d\n", temp_y); // for debug
            refresh_screen(matrix, 1);
        }
    }
    return i; // just returning the last hit x point for future hit detection.
}

int bullet_trajectory_on_x(BULLETDATA data, int x){
    // Equation : y = h + xtan(α) - gx²/2V₀²cos²(α)
    int temp_y = 0;
    temp_y = data.init_y + ((x - data.init_x) * tan(data.angle)) - (GRAVITY_CONSTANT * pow((x- data.init_x), 2)) / (2 * pow(data.velocity, 2) * pow(cos(data.angle), 2));
    
    return (int)temp_y ;
}

void set_map(char* matrix){
    for (int i = 0 ; i < GROUND_HEIGHT ;  i++)
        for (int j = 0 ; j < WIDTH ; j++) matrix[indexer(i, j)] = 1;
    
    return;
}

void set_pixel_color(char pixel_value){
    switch(pixel_value){
        case 0: // Case for Sky _ cyan (0)
        {
            printf("%s", KCYN);
            break;
        }
        case 1: // Case for Ground _ green (1)
        {
            printf("%s", KGRN);
            break;
        }
        case 2: // Case for bullet _ red (2)
        {
            printf("%s", KRED);
            break;
        }
        case 7: // Case for Tank 1 _ blue (7)
        {
            printf("%s", KBLU);
            break;
        }
        case 8: // Case for Tank 1 _ blue (7)
        {
            printf("%s", KMAG);
            break;
        }
        default: // Case for default _ white (*)
        {
            printf("%s", KWHT);
            break;
        }
    }
}

void make_screen_matrix(char** matrix){
    *matrix = (char*)malloc(sizeof(char) * WIDTH * HEIGHT);
    return;
}

void draw_screen(char* matrix){
    for (int i = HEIGHT ; i > 0 ; i--){
        for (int j = 0 ; j < WIDTH ; j++){
            set_pixel_color(matrix[indexer(i, j)]);
            printf("%d", matrix[indexer(i, j)]);
        }
        printf("\n");
    }
    printf("%s", KWHT);
    return;
}

int indexer(int y, int x){
    // OK I fucked up the coordinates.
    // Y axis start from 1 and X axis from 0. So the staring point of this matrix is actually (1,0)
    return y * WIDTH + x;
}


int main(void){
    init_game();
    return 0;
}
