#pragma once

const bool DEBUG = false;

const int GAME_FPS = 120;
const float GAME_SCALE = 0.8;
const int GAME_PROGRESSION = 3;
const int SPAWN_CHANCE = 15;
const int WINDOW_WIDTH = 1920;
const int WINDOW_HEIGHT = 1080;

const int MAX_STATIONS = 30;
const int MAX_LINES = 7;
const int MAX_TRAINS = 20;
const int MAX_PASS_STATION = 10;

const int LINE_WIDTH = 10;
const int STATION_SIZE = 30;
const int STATION_GRID = 45;
const int PASSENGER_SIZE = 12;
const int TRAIN_WIDTH = 30;
const int TRAIN_HEIGHT = 50;
const int BUTTON_SIZE = 30;
const int MENU_HEIGHT = 1020;
const int BUTTON_SIZE_BIG = 60;
const int BUTTONS_SPACING = 50;
const int MIDPOINT_CURVE = -5;
const int STATION_CURVE = -5;
const int TCAP_DISTANCE = 40;
const float TRAIN_SPEED = 0.8;
const float PI = 3.14159;

// Starting stations coordinates
const int ST_1[2] = { STATION_SIZE*2 + STATION_GRID*3*5, STATION_SIZE*2 + STATION_GRID*3*3 };
const int ST_2[2] = { STATION_SIZE*2 + STATION_GRID*3*7, STATION_SIZE*2 + STATION_GRID*3*3 };
const int ST_3[2] = { STATION_SIZE*2 + STATION_GRID*3*7, STATION_SIZE*2 + STATION_GRID*3*5 };