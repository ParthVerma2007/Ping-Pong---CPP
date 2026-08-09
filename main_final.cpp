#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <vector>

enum GameState { PLAYING, PAUSED, GAME_OVER };

int main() {
    sf::RenderWindow window(sf::VideoMode({800, 600}), "Ping Pong");
    window.setFramerateLimit(60);

    const float WINDOW_WIDTH = 800.f;
    const float WINDOW_HEIGHT = 600.f;
    const float PADDLE_SPEED = 7.0f;
    const float INITIAL_BALL_SPEED = 5.0f;
    const int MAX_SCORE = 10;
    int leftscore = 0, rightscore = 0;
    GameState currentState = GameState::PLAYING;

    // Sound setup
    sf::SoundBuffer wall;
    bool wallSoundLoaded = wall.loadFromFile("D:/Tetris/Sound effects/wall_hit.mp3");
    sf::Sound wallSound(wall);

    sf::SoundBuffer paddle;
    bool paddleSoundLoaded = paddle.loadFromFile("D:/Tetris/Sound effects/paddle_hit.mp3");
    sf::Sound paddleSound(paddle);

    sf::SoundBuffer score;
    bool scoreSoundLoaded = score.loadFromFile("D:/Tetris/Sound effects/game over.mp3");
    sf::Sound scoreSound(score);

    // Paddles Setup
    sf::Vector2f paddleSize(20.f, 100.f);

    sf::RectangleShape leftPaddle(paddleSize);
    leftPaddle.setFillColor(sf::Color::White);
    leftPaddle.setPosition({30.f, (WINDOW_HEIGHT / 2.f) - (paddleSize.y / 2.f)});

    sf::RectangleShape rightPaddle(paddleSize);
    rightPaddle.setFillColor(sf::Color::White);
    rightPaddle.setPosition({WINDOW_WIDTH - 30.f - paddleSize.x, (WINDOW_HEIGHT / 2.f) - (paddleSize.y / 2.f)});

    // Ball Setup
    float ballRadius = 10.f;
    sf::CircleShape ball(ballRadius);
    ball.setFillColor(sf::Color::White);
    ball.setPosition({(WINDOW_WIDTH / 2.f) - ballRadius, (WINDOW_HEIGHT / 2.f) - ballRadius});

    sf::Vector2f ballVelocity(INITIAL_BALL_SPEED, INITIAL_BALL_SPEED);

    // Middle dashed line setup
    std::vector<sf::RectangleShape> netDashes;
    float dashWidth = 4.f;
    float dashHeight = 15.f;
    float dashSpacing = 15.f;

    for (float y = 10.f; y < WINDOW_HEIGHT; y += dashHeight + dashSpacing) {
        sf::RectangleShape dash(sf::Vector2f(dashWidth, dashHeight));
        dash.setFillColor(sf::Color(100, 100, 100));
        dash.setPosition({(WINDOW_WIDTH / 2.f) - (dashWidth / 2.f), y});
        netDashes.push_back(dash);
    }

    // Font & UI Text Setup
    sf::Font font;
    bool fontLoaded = font.openFromFile("D:/Tetris/Arial Fond/arial.ttf"); 

    sf::Text leftScoreText(font);
    leftScoreText.setString("0");
    leftScoreText.setCharacterSize(40);
    leftScoreText.setFillColor(sf::Color::White);
    leftScoreText.setPosition({WINDOW_WIDTH / 4.f, 40.f});

    sf::Text rightScoreText(font);
    rightScoreText.setString("0");
    rightScoreText.setCharacterSize(40);
    rightScoreText.setFillColor(sf::Color::White);
    rightScoreText.setPosition({(WINDOW_WIDTH * 3.f / 4.f) - 20.f, 40.f});

    // Helper to Reset State
    auto resetGame = [&]() {
        leftscore = 0;
        rightscore = 0;
        leftPaddle.setPosition({30.f, (WINDOW_HEIGHT / 2.f) - (paddleSize.y / 2.f)});
        rightPaddle.setPosition({WINDOW_WIDTH - 30.f - paddleSize.x, (WINDOW_HEIGHT / 2.f) - (paddleSize.y / 2.f)});
        ball.setPosition({(WINDOW_WIDTH / 2.f) - ballRadius, (WINDOW_HEIGHT / 2.f) - ballRadius});
        ballVelocity = {INITIAL_BALL_SPEED, INITIAL_BALL_SPEED};
    };

    // --- Top UI Buttons ---
    float btnWidth = 70.f;
    float btnHeight = 25.f;
    float btnY = 10.f;

    sf::RectangleShape pauseBtn({btnWidth, btnHeight});
    pauseBtn.setFillColor(sf::Color(70, 130, 180));
    pauseBtn.setPosition({(WINDOW_WIDTH / 2.f) - 120.f, btnY});

    sf::Text pauseText(font);
    pauseText.setString("Pause");
    pauseText.setCharacterSize(14);
    pauseText.setFillColor(sf::Color::White);
    pauseText.setPosition({pauseBtn.getPosition().x + 12.f, pauseBtn.getPosition().y + 4.f});

    sf::RectangleShape resetBtn({btnWidth, btnHeight});
    resetBtn.setFillColor(sf::Color(200, 140, 40));
    resetBtn.setPosition({(WINDOW_WIDTH / 2.f) - 35.f, btnY});

    sf::Text resetBtnText(font);
    resetBtnText.setString("Reset");
    resetBtnText.setCharacterSize(14);
    resetBtnText.setFillColor(sf::Color::White);
    resetBtnText.setPosition({resetBtn.getPosition().x + 15.f, resetBtn.getPosition().y + 4.f});

    sf::RectangleShape quitBtn({btnWidth, btnHeight});
    quitBtn.setFillColor(sf::Color(180, 50, 50));
    quitBtn.setPosition({(WINDOW_WIDTH / 2.f) + 50.f, btnY});

    sf::Text quitText(font);
    quitText.setString("Quit");
    quitText.setCharacterSize(14);
    quitText.setFillColor(sf::Color::White);
    quitText.setPosition({quitBtn.getPosition().x + 20.f, quitBtn.getPosition().y + 4.f});

    // Game Over UI
    sf::Text gameOverText(font);
    gameOverText.setCharacterSize(50);
    gameOverText.setFillColor(sf::Color::Yellow);

    sf::RectangleShape replayButton({200.f, 50.f});
    replayButton.setFillColor(sf::Color(50, 150, 50));
    replayButton.setPosition({(WINDOW_WIDTH / 2.f) - 100.f, (WINDOW_HEIGHT / 2.f) + 30.f});

    sf::Text replayText(font);
    replayText.setString("REPLAY");
    replayText.setCharacterSize(25);
    replayText.setFillColor(sf::Color::White);
    replayText.setPosition({replayButton.getPosition().x + 50.f, replayButton.getPosition().y + 10.f});

    // Main Game Loop
    while (window.isOpen()) {
        while (const auto event = window.pollEvent()) {
            if (event->is<sf::Event::Closed>()) {
                window.close();
            }

            if (const auto* mouseClick = event->getIf<sf::Event::MouseButtonPressed>()) {
                if (mouseClick->button == sf::Mouse::Button::Left) {
                    sf::Vector2f mousePos(static_cast<float>(mouseClick->position.x), static_cast<float>(mouseClick->position.y));

                    // Quit Button
                    if (quitBtn.getGlobalBounds().contains(mousePos)) {
                        window.close();
                    }

                    // Pause / Resume Button
                    if (pauseBtn.getGlobalBounds().contains(mousePos)) {
                        if (currentState == GameState::PLAYING) {
                            currentState = GameState::PAUSED;
                            pauseText.setString("Resume");
                        } else if (currentState == GameState::PAUSED) {
                            currentState = GameState::PLAYING;
                            pauseText.setString("Pause");
                        }
                    }

                    // Reset Button
                    if (resetBtn.getGlobalBounds().contains(mousePos)) {
                        resetGame();
                        currentState = GameState::PLAYING;
                        pauseText.setString("Pause");
                    }

                    // Replay Button (Game Over State)
                    if (currentState == GameState::GAME_OVER && replayButton.getGlobalBounds().contains(mousePos)) {
                        resetGame();
                        currentState = GameState::PLAYING;
                        pauseText.setString("Pause");
                    }
                }
            }
        }

        // UPDATE GAME LOGIC ONLY IN PLAYING STATE
        if (currentState == GameState::PLAYING) {
            // Left paddle
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::W) && leftPaddle.getPosition().y > 0.f) {
                leftPaddle.move({0.f, -PADDLE_SPEED});
            }
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::S) && leftPaddle.getPosition().y + paddleSize.y < WINDOW_HEIGHT) {
                leftPaddle.move({0.f, PADDLE_SPEED});
            }

            // Right paddle
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Up) && rightPaddle.getPosition().y > 0.f) {
                rightPaddle.move({0.f, -PADDLE_SPEED});
            }
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Down) && rightPaddle.getPosition().y + paddleSize.y < WINDOW_HEIGHT) {
                rightPaddle.move({0.f, PADDLE_SPEED});
            }

            // Ball movement
            ball.move(ballVelocity);

            if (ball.getPosition().y <= 0.f) {
                ball.setPosition({ball.getPosition().x, 0.f});
                ballVelocity.y = -ballVelocity.y;
                if (wallSoundLoaded) wallSound.play();
            }

            if (ball.getPosition().y + (ballRadius * 2.f) >= WINDOW_HEIGHT) {
                ball.setPosition({ball.getPosition().x, WINDOW_HEIGHT - (ballRadius * 2.f)});
                ballVelocity.y = -ballVelocity.y;
                if (wallSoundLoaded) wallSound.play();
            }

            // Collisions
            if (ballVelocity.x < 0.f && ball.getGlobalBounds().findIntersection(leftPaddle.getGlobalBounds())) {
                ball.setPosition({leftPaddle.getPosition().x + paddleSize.x, ball.getPosition().y});
                ballVelocity.x = -ballVelocity.x * 1.05f;
                ballVelocity.y *= 1.05f;
                if (paddleSoundLoaded) paddleSound.play();
            }

            if (ballVelocity.x > 0.f && ball.getGlobalBounds().findIntersection(rightPaddle.getGlobalBounds())) {
                ball.setPosition({rightPaddle.getPosition().x - (ballRadius * 2.f), ball.getPosition().y});
                ballVelocity.x = -ballVelocity.x * 1.05f;
                ballVelocity.y *= 1.05f;
                if (paddleSoundLoaded) paddleSound.play();
            }

            // Scoring
            if (ball.getPosition().x < 0.f) {
                rightscore++;
                if (scoreSoundLoaded) scoreSound.play();
                ball.setPosition({(WINDOW_WIDTH / 2.f) - ballRadius, (WINDOW_HEIGHT / 2.f) - ballRadius});
                rightPaddle.setPosition({WINDOW_WIDTH - 30.f - paddleSize.x, (WINDOW_HEIGHT / 2.f) - (paddleSize.y / 2.f)});
                leftPaddle.setPosition({30.f, (WINDOW_HEIGHT / 2.f) - (paddleSize.y / 2.f)});
                ballVelocity = {INITIAL_BALL_SPEED, INITIAL_BALL_SPEED};
            }

            if (ball.getPosition().x + (ballRadius * 2.f) > WINDOW_WIDTH) {
                leftscore++;
                if (scoreSoundLoaded) scoreSound.play();
                ball.setPosition({(WINDOW_WIDTH / 2.f) - ballRadius, (WINDOW_HEIGHT / 2.f) - ballRadius});
                rightPaddle.setPosition({WINDOW_WIDTH - 30.f - paddleSize.x, (WINDOW_HEIGHT / 2.f) - (paddleSize.y / 2.f)});
                leftPaddle.setPosition({30.f, (WINDOW_HEIGHT / 2.f) - (paddleSize.y / 2.f)});
                ballVelocity = {-INITIAL_BALL_SPEED, INITIAL_BALL_SPEED};
            }

            // Win condition
            if (leftscore >= MAX_SCORE || rightscore >= MAX_SCORE) {
                currentState = GameState::GAME_OVER;
                if (leftscore >= MAX_SCORE) {
                    gameOverText.setString("PLAYER 1 WINS!");
                } else {
                    gameOverText.setString("PLAYER 2 WINS!");
                }
                gameOverText.setPosition({(WINDOW_WIDTH / 2.f) - 200.f, (WINDOW_HEIGHT / 2.f) - 80.f});
            }

            leftScoreText.setString(std::to_string(leftscore));
            rightScoreText.setString(std::to_string(rightscore));
        }

        // DRAW EVERYTHING
        window.clear(sf::Color(20, 20, 20, 255));

        for (const auto& dash : netDashes) {
            window.draw(dash);
        }

        if (fontLoaded) {
            window.draw(leftScoreText);
            window.draw(rightScoreText);

            // Draw Top UI Buttons
            window.draw(pauseBtn);
            window.draw(pauseText);

            window.draw(resetBtn);
            window.draw(resetBtnText);

            window.draw(quitBtn);
            window.draw(quitText);
        }

        window.draw(leftPaddle);
        window.draw(rightPaddle);
        window.draw(ball);

        if (currentState == GameState::GAME_OVER && fontLoaded) {
            window.draw(gameOverText);
            window.draw(replayButton);
            window.draw(replayText);
        }

        window.display();
    }

    return 0;
}
