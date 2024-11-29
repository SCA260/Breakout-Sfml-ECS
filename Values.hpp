#ifndef VALUES_HPP
#define VALUES_HPP

// window 
const float window_height = 800.f;
const float window_width = 600.f;	
//ball
float ball_size = 40.f;
float ball_initial_x = window_width / 2 - ball_size / 2;
float ball_initial_y = window_height / 2 - ball_size / 2;
int fallen_balls = 0;

//bricks
int destroyed_bricks = 0;
int brick_rows = 5;
int brick_cols = 8;
float brick_width = 60.f;
float brick_height = 20.f;
float brick_offset_x = 20.f;
float brick_offset_y = 50.f;
float brick_spacing = 10.f;

#endif