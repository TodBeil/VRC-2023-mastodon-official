// Import headers của các thư viện
#include <PS2X_lib.h>
#include <Adafruit_PWMServoDriver.h>
#include <Wire.h>

// Set up các chân tín hiệu của tay cầm PS2
#define PS2_DAT 12
#define PS2_CMD 13
#define PS2_CLK 14
#define PS2_ATT 15

// Kênh PWM cho chân thuận (Tay phải)
#define PWM_DC1A 14
#define PWM_DC1B 15
#define PWM_DC4A 12
#define PWM_DC4B 13
// Kenh PWM cho chân ngược (Tay trái)
#define PWM_DC2A 8
#define PWM_DC2B 9
#define PWM_DC3A 10
#define PWM_DC3B 11

// Động cơ DC
#define MOT_LEFT 1
#define MOT_RIGHT 2
#define MOT_INTAKE_OUTTAKE 3

// set tốc độ động cơ
#define SPD_FAST 2047
#define SPD_MAX 4095


// Kenh PWM cho servo
#define PWM_SERVO0 2
#define PWM_SERVO1 3
#define PWM_SERVO2 4
#define PWM_SERVO3 5
#define PWM_SERVO4 6
#define PWM_SERVO5 7

// Cổng của servo
#define SRV_SHOOTER 7

// Khai báo linh kiện
Adafruit_PWMServoDriver pwm; 
PS2X ps2;

void setup()
{                         // Hàm set up chạy khởi tạo một lần 
    Serial.begin(115200); // Bật serial monitor 

    Serial.println("Mastodon's BanhMi Connected To PC. Proceeding to connect PS2 Controller");

    while (ps2.config_gamepad(PS2_CLK, PS2_CMD, PS2_ATT, PS2_DAT) != 0)
    {
        Serial.println("Err: PS2 Controller Not Connected");
        delay(500);
    } // Kiểm tra kết nối của các chân tín hiệu của điều khiểnyu

    Serial.println("Controller works normally");

    pwm.begin();                          // Khởi tạo xung pwm
    pwm.setOscillatorFrequency(27000000); // Đặt tần số xung trong PCA9685 là 27000000 (27MHz)
    pwm.setPWMFreq(50);                   // Đặt tần số giao động trên chân tối đa 50Hz 


// Hàm điều khiển động cơ DC (nhận số động cơ từ 1->4 ứng với cặp kênh PWM 8-9/10-11/12-13/14-15, giá trị tốc độ từ -4095 đến 4095)
void ctrl_dc(uint8_t motor, int16_t speed)
{
    switch (motor)
    {
    case 1:                                                 
        pwm.setPWM(PWM_DC1A, 0, ((speed > 0) ? speed : 0)); 
        pwm.setPWM(PWM_DC1B, 0, ((speed < 0) ? (-speed) : 0));
        break;
    case 2: // Cho bánh bên tay trái
        pwm.setPWM(PWM_DC2A, 0, ((speed > 0) ? speed : 0));
        pwm.setPWM(PWM_DC2B, 0, ((speed < 0) ? (-speed) : 0));
        break;
    case 3: // Cho bánh bên tay phải
        pwm.setPWM(PWM_DC3A, 0, ((speed > 0) ? speed : 0));
        pwm.setPWM(PWM_DC3B, 0, ((speed < 0) ? (-speed) : 0));
        break;
    case 4:
        pwm.setPWM(PWM_DC4A, 0, ((speed > 0) ? speed : 0));
        pwm.setPWM(PWM_DC4B, 0, ((speed < 0) ? (-speed) : 0));y
        break;
    }
}

// Hàm điều khiển vị trí servo 180 (nhận số động cơ servo từ 1->5 ứng với kênh PWM 1->7)
void ctrl_servo180(uint8_t motor, float angle)
{
    uint16_t us = (uint16_t)(1000 * (1.0 + angle / 180.0));
    switch (motor)
    {
    case 1:
        pwm.writeMicroseconds(PWM_SERVO0, us);
        break; // writeMicrosecond dùng để cho servo biết phải quay bao nhiêu ms để được 1 góc
    case 2:
        pwm.writeMicroseconds(PWM_SERVO1, us);
        break;
    case 3:
        pwm.writeMicroseconds(PWM_SERVO2, us);
        break;
    case 4:
        pwm.writeMicroseconds(PWM_SERVO3, us);
        break;
    case 5:
        pwm.writeMicroseconds(PWM_SERVO4, us);
        break;
    }
}

void loop()
{
    ps2.read_gamepad(); // Khởi tạo đọc từ điều kiển bằng hàm read_gamepad()

    //điều khiển hai motor di chuyển
    ctrl_dc(MOT_LEFT, map(ps2.Analog(PSS_LY), 0, 255, -SPD_FAST, SPD_FAST));
    ctrl_dc(MOT_RIGHT, map(ps2.Analog(PSS_RY), 0, 255, SPD_FAST, -SPD_FAST));

    //điều khiển motor intake/outtake
    if (ps2.Button(PSB_L2))
    {
        ctrl_dc(MOT_INTAKE_OUTTAKE, (true) ? -SPD_MAX);
    }
    if (ps2.Button(PSB_R2))
    {
        ctrl_dc(MOT_INTAKE_OUTTAKE, (true) ? SPD_MAX);
    }
    if (ps2.Button(PSB_R1) || ps2.Button(PSB_L1))
    {
        ctrl_dc(MOT_INTAKE_OUTTAKE, (true) ? 0);
    }
    //điều khiển servo tay bắn
    if (PSB_PAD_UP)
    {
        ctrl_servo180(0, 100);
    }
    //nếu bấm pad up không tạo ra đủ lực căng để kéo chốt thì bấm pad down cho servo quay thêm tạo ra lực căng
    if (PSB_PAD_DOWN)
    {
        ctrl_servo180(0, -100);
    }
}
