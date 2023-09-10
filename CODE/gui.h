// GUI header file 
#ifndef GUI_H
#define GUI_H

#include"raylib.h"

typedef struct Button{
    Vector2 position;
    Vector2 size;
    bool hover;
    bool clicked;   
}Button;

// typedef struct Circle
// {
//     float radius;
//     Vector2 center;
// }Circle;


bool ButtonHover(Vector2 Button_position,Vector2 Button_size,Vector2 Mouse_coordinates)
{
    if(Mouse_coordinates.x<=Button_position.x+Button_size.x && Mouse_coordinates.x>=Button_position.x && Mouse_coordinates.y>=Button_position.y && Mouse_coordinates.y<=Button_position.y+Button_size.y)
    {
        return true;
    }
    else return false;
}

bool ButtonClicked(Vector2 Button_position,Vector2 Button_size,Vector2 Mouse_coordinates)
{
    if(Mouse_coordinates.x<=Button_position.x+Button_size.x && Mouse_coordinates.x>=Button_position.x && Mouse_coordinates.y>=Button_position.y && Mouse_coordinates.y<=Button_position.y+Button_size.y && IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
    {
        return true;
    }
    else return false;
}

#endif