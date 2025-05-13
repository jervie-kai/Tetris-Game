#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <vector>
#include <time.h>
#include <iostream>
#include <vector>
#include <cstring>
using namespace std;
using namespace sf;

const int tileSize = 30;
const int height = 22;
const int width = 12;
int board[height][width] = {0};

struct Block{
    int x,y;
};
Block ab[4],bb[4];

class TetrisGame{
private:
    RenderWindow window;
    Texture blockTexture, backgroundTexture;
    Sprite blockSprite, backgroundSprite, sprite;
    Font font, font1;
    Text textScore, textLevel, game, over, textLine, restart;
    Music backgroundMusic;
    int dir = 0;
    float timer = 0.0f, delay = 0.4f;
    bool hardDrop = false;
    bool gameOver = false;
    bool gameStart = false;
    int blocksDraw = 1;
    int score = 0;
    int lines = 0;
    int level = 1;
    int r;
    string playerName;
    Clock clock;

    int shapes[7][4][2] = {
        {{0,1},{1,1},{2,1},{3,1}},//I
        {{2,0},{0,1},{1,1},{2,1}},//J
        {{0,0},{0,1},{1,1},{2,1}},//L
        {{0,0},{1,0},{0,1},{1,1}},//O
        {{1,0},{2,0},{0,1},{1,1}},//S
        {{1,0},{0,1},{1,1},{2,1}},//T
        {{0,0},{1,0},{1,1},{2,1}},//Z
    };

    void handlingEvents(){
        Event event;
        while(window.pollEvent(event)){
            if(event.type == Event::Closed){
                window.close();
            }
            if(event.type == Event::KeyPressed){
                if(!gameStart && event.key.code == Keyboard::Enter){
                    gameStart = true;
                    newBlock();
                    return;
                }
                if(!gameOver && gameStart){
                    if(event.key.code == Keyboard::Down){
                        delay = 0.4f;
                    }else if(event.key.code == Keyboard::Right){
                        dir = 1;
                    }else if(event.key.code == Keyboard::Left){
                        dir = -1;
                    }else if(event.key.code == Keyboard::Up){
                        rotateBlock();
                    }else if(event.key.code == Keyboard::Space){
                        hardDrop = true;
                    }
                }
                if(event.key.code == Keyboard::Escape){
                    window.close();
                }
                if(event.key.code == Keyboard::R){
                    reset();
                }
            }
        }
    }
    void rotateBlock(){
        Block b = ab[1];
        for(int i = 0; i < 4; i++){
            int x = ab[i].x - b.x;
            int y = ab[i].y - b.y;

            bb[i].x = b.x - y;
            bb[i].y = b.y - x;
        }
        if(!isCollision(bb)){
            for(int i = 0; i < 4; i++){
                ab[i] = bb[i];
            }
        }
    }
    void hardDropBlock(){
        while(true){
            for(int i = 0; i < 4; i++){
                bb[i] = ab[i];
            }
            for(int i = 0; i < 4; i++){
                ab[i].y += 1;
            }
            if(checkCollision()){
                for(int i = 0; i < 4; i++){
                    ab[i] = bb[i];
                }
                break;
            }
        }
        lockBlock();
    }
    void lockBlock(){
        for(int i=0;i<4;i++){
            if(ab[i].x <0 || ab[i].y >= height || ab[i].x >= width){
                gameOver = true;
                return;
            }
            board[ab[i].y][ab[i].x] = blocksDraw;
        }
        lineClear();
        newBlock();
        if(checkCollision()){
            gameOver = true;
        }
    }
    void drawBlock(){
        for(int i=0;i<4;i++){
            blockSprite.setTextureRect(IntRect(blocksDraw * tileSize, 0, tileSize, tileSize));
            blockSprite.setPosition(ab[i].x * tileSize, ab[i].y * tileSize);
            window.draw(blockSprite);
        }
    }
    void newBlock(){
        r = rand() % 7;
        blocksDraw = 1 + r;
        for(int i = 0; i < 4; i++){
            ab[i].x = shapes[r][i][0] + width / 2 -2;
            ab[i].y = shapes[r][i][1];
        }
        if(checkCollision()){
            gameOver = true;
        }
    }
    void lineClear() {
        for (int i = height - 1; i >= 0; i--) {
            int columnCounter = 0;
            for (int j = 0; j < width; j++) {
                if (board[i][j]) columnCounter++;
            }
            if (columnCounter == width) {
                for (int k = i; k > 0; k--) {
                    for (int j = 0; j < width; j++) {
                        board[k][j] = board[k - 1][j];
                    }
                }
                for (int j = 0; j < width; j++) board[0][j] = 0;

                score += 50;
                lines++;

                i++;
                if (lines % 5 == 0) {
                    level++;
                    delay = max(0.1f, delay - 0.05f);
                }
            }
        }
    }
    bool isCollision(Block block[4]){
        for(int i = 0; i < 4; i++){
            int x = block[i].x;
            int y = block[i].y;
            if(x < 0 || x >= width || y >= height) {
                return true;
            }
            if(y >= 0 && board[y][x]){
                return true;
            }
        }
        return false;
    }
    bool checkCollision(){
        return isCollision(ab);
    }
    void blockMove(){
        for(int i = 0; i < 4; i++) {
            bb[i] = ab[i];
        }
        for(int i = 0; i < 4; i++) {
            ab[i].x += dir;
        }
        if(checkCollision()){
            for(int i = 0; i < 4; i++) ab[i] = bb[i];
        }
    }
    void drawAlreadyPlaceBlocks(){
        for(int i=0;i<height;i++){
            for(int j=0;j<width;j++){
                if(board[i][j] == 0) continue;
                sprite.setTextureRect(IntRect(board[i][j] * tileSize, 0, tileSize, tileSize));
                sprite.setPosition(j * tileSize, i * tileSize);
                sprite.setTexture(blockTexture);
                window.draw(sprite);
            }
        }
    }
    void drawGrid(){
        Color gridColor(255, 255, 255, 50);
        for(int i = 0; i <= width; i++){
            Vertex line[] = {
                Vertex(Vector2f(i * tileSize, 0), gridColor),
                Vertex(Vector2f(i * tileSize, height * tileSize), gridColor)
            };
            window.draw(line, 2, Lines);
        }
        for(int i = 0; i <= height; i++){
            Vertex line[] = {
                Vertex(Vector2f(0, i * tileSize), gridColor),
                Vertex(Vector2f(width * tileSize, i * tileSize), gridColor)
            };
            window.draw(line, 2, Lines);
        }
    }
    void reset(){
        score = 0;
        level = 1;
        delay = 0.4f;
        lines = 0;
        dir = 0;
        gameOver = false;
        hardDrop = false;
        memset(board, 0, sizeof(board));
        newBlock();
    }
    void draw(){
        window.clear();
        window.draw(backgroundSprite);

        //Score
        textScore.setFont(font);
        textScore.setString("Score: " + to_string(score));
        textScore.setCharacterSize(20);
        textScore.setFillColor(Color::White);
        textScore.setPosition(width * tileSize + 15, 100);
        window.draw(textScore);

        //Restart
        restart.setFont(font);
        restart.setString("R to Restart \nEsc to quit");
        restart.setCharacterSize(20);
        restart.setFillColor(Color::White);
        restart.setPosition(width * tileSize + 15, 250);
        window.draw(restart);

        drawGrid();
        drawAlreadyPlaceBlocks();

        if(!gameStart){
            game.setFont(font1);
            game.setString("Welcome to Asian College Tetris Game \n           Press Enter to start \n           Press Esc to quit");
            game.setFillColor(Color::White);
            game.setCharacterSize(30);
            game.setOutlineColor(Color::Black);
            game.setOutlineThickness(2);
            FloatRect textRect = game.getLocalBounds();
            game.setOrigin(textRect.width / 2, textRect.height / 2 + 30);
            game.setPosition(window.getSize().x / 2.0f, window.getSize().y / 2.0f);
            window.draw(game);
        }

        if(gameOver){
            over.setFont(font1);
            over.setString("      Game Over \n Press R to restart");
            over.setFillColor(Color::Red);
            over.setCharacterSize(50);
            over.setOutlineColor(Color::Black);
            over.setOutlineThickness(2);
            FloatRect overRect = game.getLocalBounds();
            over.setOrigin(overRect.width / 2 - 30, overRect.height / 2 + 40);
            over.setPosition(window.getSize().x / 2.0f, window.getSize().y / 2.0f);
            window.draw(over);
        }

        //Level
        textLevel.setFont(font);
        textLevel.setString("Level: " + to_string(level));
        textLevel.setCharacterSize(20);
        textLevel.setFillColor(Color::White);
        textLevel.setPosition(width * tileSize + 15, 150);
        window.draw(textLevel);

        //Line
        textLine.setFont(font);
        textLine.setString("Lines break: " + to_string(lines));
        textLine.setCharacterSize(20);
        textLine.setFillColor(Color::White);
        textLine.setPosition(width * tileSize + 10, 200);
        window.draw(textLine);

        drawBlock();
        window.display();
    }
public:
    TetrisGame() : window(VideoMode(width * tileSize + 150, height * tileSize), "Asian College Tetris"){
        srand(time(0));
        if(!font.loadFromFile("OpenSans-Regular.ttf")) {
            cout<<"Unable to load font";
        }
        if(!font1.loadFromFile("Fake-Red-Nation.ttf")) {
            cout<<"Unable to load font";
        }
        if(!blockTexture.loadFromFile("blocks.png")) {
            cout<<"Failed to load tiles";
        }

        blockSprite.setTexture(blockTexture);
        backgroundTexture.loadFromFile("backgroundImage.png");
        backgroundSprite.setTexture(backgroundTexture);

        Vector2u backgroundSize = backgroundTexture.getSize();
        Vector2u windowSize = window.getSize();

        float scaleX = static_cast<float>(windowSize.x) / backgroundSize.x;
        float scaleY = static_cast<float>(windowSize.y) / backgroundSize.y;

        backgroundSprite.setScale(scaleX, scaleY);

        if(!backgroundMusic.openFromFile("asianCollegeHymn.ogg")) {
            cout<<"Failed to load background music \n";
        }
        backgroundMusic.setLoop(true);
        backgroundMusic.play();
    }
    void run() {
        while (window.isOpen()) {
            float time = clock.restart().asSeconds();
            timer += time;
            handlingEvents();
            draw();
            if (gameStart && !gameOver) {
                blockMove();
                dir = 0;
                if (hardDrop) {
                    hardDropBlock();
                    timer = 0;
                    hardDrop = false;
                    continue;
                }
                if (timer > delay) {
                    for (int i = 0; i < 4; i++) {
                        bb[i] = ab[i];
                    }
                    for (int i = 0; i < 4; i++) {
                        ab[i].y += 1;
                    }
                    if (checkCollision()) {
                        for (int i = 0; i < 4; i++) ab[i] = bb[i];
                        lockBlock();
                    }
                    timer = 0;
                }
            }
        }
    }
};

int main()
{
    TetrisGame tetris;
    tetris.run();
    return 0;
}
