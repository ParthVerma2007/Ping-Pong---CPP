#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <vector>

enum GameState {PLAYING, GAME_OVER};

int main() {
    sf::RenderWindow window(sf::VideoMode({800, 600}), "Ping Pong");
    window.setFramerateLimit(60);

    // 2. Define Dimensions
    const float WINDOW_WIDTH = 800.f;
    const float WINDOW_HEIGHT = 600.f;
    const float PADDLE_SPEED =7.0f;
    const float INITIAL_BALL_SPEED = 5.0f;
    const int MAX_SCORE = 10;
    int leftscore=0, rightscore=0;
    GameState currentState = GameState::PLAYING;

    //sound setup
    sf::SoundBuffer wall;
    bool wallSoundLoaded = wall.loadFromFile("D:/Tetris/Sound effects/wall_hit.mp3");
    sf::Sound wallSound(wall);

    sf::SoundBuffer paddle;
    bool paddleSoundLoaded = paddle.loadFromFile("D:/Tetris/Sound effects/paddle_hit.mp3");
    sf::Sound paddleSound(paddle);

    sf::SoundBuffer score;
    bool scoreSoundLoaded = score.loadFromFile("D:/Tetris/Sound effects/game over.mp3");
    sf::Sound scoreSound(score);


    // 3. Paddles Setup
    sf::Vector2f paddleSize(20.f, 100.f);

    sf::RectangleShape leftPaddle(paddleSize);
    leftPaddle.setFillColor(sf::Color::White);
    leftPaddle.setPosition({30.f, (WINDOW_HEIGHT / 2.f) - (paddleSize.y / 2.f)});

    sf::RectangleShape rightPaddle(paddleSize);
    rightPaddle.setFillColor(sf::Color::White);
    rightPaddle.setPosition({WINDOW_WIDTH - 30.f - paddleSize.x, (WINDOW_HEIGHT / 2.f) - (paddleSize.y / 2.f)});

    // 4. Ball Setup
    float ballRadius = 10.f;
    sf::CircleShape ball(ballRadius);
    ball.setFillColor(sf::Color::White);
    ball.setPosition({(WINDOW_WIDTH / 2.f) - ballRadius, (WINDOW_HEIGHT / 2.f) - ballRadius});

    sf::Vector2f ballVelocity(INITIAL_BALL_SPEED, INITIAL_BALL_SPEED);

    //middle dashed line setup
    std::vector<sf::RectangleShape> netDashes;
    float dashWidth = 4.f;
    float dashHeight = 15.f;
    float dashSpacing = 15.f;

    for (float y = 10.f; y < WINDOW_HEIGHT; y += dashHeight + dashSpacing) {
        sf::RectangleShape dash(sf::Vector2f(dashWidth, dashHeight));
        dash.setFillColor(sf::Color(100, 100, 100)); // Dim grey
        dash.setPosition({(WINDOW_WIDTH / 2.f) - (dashWidth / 2.f), y});
        netDashes.push_back(dash);
    }

    // --- NEW: 6. Font & UI Text Setup ---
    sf::Font font;
    // Note: Make sure a font file (e.g., "arial.ttf") exists in your project directory
    bool fontLoaded = font.openFromFile("arial.ttf"); 

    sf::Text leftScoreText(font);
    leftScoreText.setString("0");
    leftScoreText.setCharacterSize(40);
    leftScoreText.setFillColor(sf::Color::White);
    leftScoreText.setPosition({WINDOW_WIDTH / 4.f, 20.f});

    sf::Text rightScoreText(font);
    rightScoreText.setString("0");
    rightScoreText.setCharacterSize(40);
    rightScoreText.setFillColor(sf::Color::White);
    rightScoreText.setPosition({(WINDOW_WIDTH * 3.f / 4.f) - 20.f, 20.f});

    //game over and replay game
    sf::Text gameOverText(font);
    gameOverText.setCharacterSize(50);
    gameOverText.setFillColor(sf::Color::Yellow);

    sf::RectangleShape replayButton({200.f, 50.f});
    replayButton.setFillColor(sf::Color(50, 150, 50)); // Green
    replayButton.setPosition({(WINDOW_WIDTH / 2.f) - 100.f, (WINDOW_HEIGHT / 2.f) + 30.f});

    sf::Text replayText(font);
    replayText.setString("REPLAY");
    replayText.setCharacterSize(25);
    replayText.setFillColor(sf::Color::White);
    replayText.setPosition({replayButton.getPosition().x + 50.f, replayButton.getPosition().y + 10.f});
    
    // 5. Main Game Loop
    while (window.isOpen()) {
        // Event handling
        while (const auto event = window.pollEvent()) {
            if (event->is<sf::Event::Closed>()) {
                window.close();
            }
        // Check Replay Button Clicks in Game Over State
            if (currentState == GameState::GAME_OVER) {
           if (const auto* mouseClick = event->getIf<sf::Event::MouseButtonPressed>()) {
                    if (mouseClick->button == sf::Mouse::Button::Left) {
                        sf::Vector2f mousePos(static_cast<float>(mouseClick->position.x), static_cast<float>(mouseClick->position.y));

                        // If user clicked inside the Replay Button
                        if (replayButton.getGlobalBounds().contains(mousePos)) {
                            // Reset Game Logic
                            leftscore = 0;
                            rightscore = 0;
                            leftPaddle.setPosition({30.f, (WINDOW_HEIGHT / 2.f) - (paddleSize.y / 2.f)});
                            rightPaddle.setPosition({WINDOW_WIDTH - 30.f - paddleSize.x, (WINDOW_HEIGHT / 2.f) - (paddleSize.y / 2.f)});
                            ball.setPosition({(WINDOW_WIDTH / 2.f) - ballRadius, (WINDOW_HEIGHT / 2.f) - ballRadius});
                            ballVelocity = {INITIAL_BALL_SPEED, INITIAL_BALL_SPEED};
                            
                            currentState = GameState::PLAYING; // Unpause game
                        }
                    }
                }
            }
        }

        if (currentState == GameState::PLAYING) {
        //logic
        //input
        //left paddle
        if(sf::Keyboard::isKeyPressed(sf::Keyboard::Key::W) && leftPaddle.getPosition().y>0.f){
            leftPaddle.move({0.f, -PADDLE_SPEED});
        }

        if(sf::Keyboard::isKeyPressed(sf::Keyboard::Key::S) && leftPaddle.getPosition().y+paddleSize.y < WINDOW_HEIGHT){
            leftPaddle.move({0.f, PADDLE_SPEED});
        }

        //right paddle
        if(sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Up) && rightPaddle.getPosition().y>0.f){
            rightPaddle.move({0.f, -PADDLE_SPEED});
        }

        if(sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Down) && rightPaddle.getPosition().y+paddleSize.y < WINDOW_HEIGHT){
            rightPaddle.move({0.f, PADDLE_SPEED});
        }

        //ball velocity and movement
        ball.move(ballVelocity);
        if(ball.getPosition().y<=0.f){
            ball.setPosition({ball.getPosition().x,0.f});
            ballVelocity.y = -ballVelocity.y;
            if(wallSoundLoaded) wallSound.play();
        }

        if(ball.getPosition().y + (ballRadius*2.f) >=WINDOW_HEIGHT){
            ball.setPosition({ball.getPosition().x, WINDOW_HEIGHT-(ballRadius*2.f)});
            ballVelocity.y = -ballVelocity.y;
            if(wallSoundLoaded) wallSound.play();
        }

        //paddle-ball collision
        // Left Paddle Collision (moving left towards player 1)
        if (ballVelocity.x < 0.f && ball.getGlobalBounds().findIntersection(leftPaddle.getGlobalBounds())) {
            // Push ball to the right edge of left paddle to prevent sticking
            ball.setPosition({leftPaddle.getPosition().x + paddleSize.x, ball.getPosition().y});
            ballVelocity.x = -ballVelocity.x; // Reverse horizontal direction
            ballVelocity.x *= 1.05f;
            ballVelocity.y *= 1.05f;
            if(paddleSoundLoaded) paddleSound.play();
        }

        // Right Paddle Collision (moving right towards player 2)
        if (ballVelocity.x > 0.f && ball.getGlobalBounds().findIntersection(rightPaddle.getGlobalBounds())) {
            // Push ball to the left edge of right paddle to prevent sticking
            ball.setPosition({rightPaddle.getPosition().x - (ballRadius * 2.f), ball.getPosition().y});
            ballVelocity.x = -ballVelocity.x; // Reverse horizontal direction
            ballVelocity.x *= 1.05f;
            ballVelocity.y *= 1.05f;
            if(paddleSoundLoaded) paddleSound.play();
        }

        //scoring and reset the ball
        // --- NEW: E. SCORING & BALL RESET ---

        // Ball goes past Left side -> Right player scores
        if (ball.getPosition().x < 0.f) {
            rightscore++;
            if (scoreSoundLoaded) scoreSound.play();
            // Reset position to center
            ball.setPosition({(WINDOW_WIDTH / 2.f) - ballRadius, (WINDOW_HEIGHT / 2.f) - ballRadius});
            rightPaddle.setPosition({WINDOW_WIDTH - 30.f - paddleSize.x, (WINDOW_HEIGHT / 2.f) - (paddleSize.y / 2.f)});
            leftPaddle.setPosition({30.f, (WINDOW_HEIGHT / 2.f) - (paddleSize.y / 2.f)});
            // Reset speed and serve towards the player who just scored
            ballVelocity = {INITIAL_BALL_SPEED, INITIAL_BALL_SPEED};
        }

        // Ball goes past Right side -> Left player scores
        if (ball.getPosition().x + (ballRadius * 2.f) > WINDOW_WIDTH) {
            leftscore++;
            if (scoreSoundLoaded) scoreSound.play();
            // Reset position to center
            ball.setPosition({(WINDOW_WIDTH / 2.f) - ballRadius, (WINDOW_HEIGHT / 2.f) - ballRadius});
            rightPaddle.setPosition({WINDOW_WIDTH - 30.f - paddleSize.x, (WINDOW_HEIGHT / 2.f) - (paddleSize.y / 2.f)});
            leftPaddle.setPosition({30.f, (WINDOW_HEIGHT / 2.f) - (paddleSize.y / 2.f)});
            // Reset speed and serve towards the player who just scored
            ballVelocity = {-INITIAL_BALL_SPEED, INITIAL_BALL_SPEED};
        }
        // --- NEW: Check Max Score Win Condition ---
            if (leftscore >= MAX_SCORE || rightscore >= MAX_SCORE) {
                currentState = GameState::GAME_OVER; // Pause game logic

                if (leftscore >= MAX_SCORE) {
                    gameOverText.setString("PLAYER 1 WINS!");
                } else {
                    gameOverText.setString("Player 2 WINS!");
                }
                
                // Center the Game Over text dynamically
                gameOverText.setPosition({(WINDOW_WIDTH / 2.f) - 200.f, (WINDOW_HEIGHT / 2.f) - 80.f});
            }
        // Update score strings
        leftScoreText.setString(std::to_string(leftscore));
        rightScoreText.setString(std::to_string(rightscore));

        // --- RENDER ---
        // Clear screen with a dark color
        window.clear(sf::Color(20, 20, 20, 255));
        for (const auto& dash : netDashes) {
            window.draw(dash);
        }

        // Draw UI scores (if font loaded successfully)
        if (fontLoaded) {
            window.draw(leftScoreText);
            window.draw(rightScoreText);
        }

        // Draw entities
        window.draw(leftPaddle);
        window.draw(rightPaddle);
        window.draw(ball);
        if (currentState == GameState::GAME_OVER && fontLoaded) {
            window.draw(gameOverText);
            window.draw(replayButton);
            window.draw(replayText);
        }
        // Display buffer
        window.display();
    }
    }
    return 0;
}
