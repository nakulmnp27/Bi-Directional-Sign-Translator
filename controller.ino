#include <WiFi.h>
#include <HTTPClient.h>
#include <Wire.h>
#include <MPU6050.h>

MPU6050 mpu;

// Flex sensor pins
int flexPins[5] = {34, 35, 32, 33, 25};
int flexValues[5];

// WiFi credentials
const char* ssid = "YOUR_SSID";
const char* password = "YOUR_PASSWORD";

// Flask API endpoint
String serverURL = "http://YOUR_SERVER_IP:5000/gesture";

// Gesture struct
struct Gesture {
  int id;
  String text;
};

// 200+ gestures with placeholder thresholds (rough thresholds for demo)
Gesture gestures[] = {
  {0,"a"},{1,"afternoon"},{2,"all"},{3,"annually"},{4,"any"},{5,"anyone"},{6,"anything"},{7,"april"},{8,"awesome"},{9,"b"},
  {10,"best friends"},{11,"best_friends"},{12,"black ice"},{13,"blizzard"},{14,"breezy"},{15,"brisk"},{16,"c"},{17,"clear skies"},{18,"cloudy"},{19,"cold"},
  {20,"cool"},{21,"dawn"},{22,"day"},{23,"december"},{24,"degrees"},{25,"direction"},{26,"dry"},{27,"dusk"},{28,"dust storm"},{29,"e"},
  {30,"each"},{31,"early"},{32,"either"},{33,"else"},{34,"engine_trouble"},{35,"evening"},{36,"every friday"},{37,"every monday"},{38,"every thursday"},{39,"every two months"},
  {40,"every two weeks"},{41,"every two years"},{42,"every wednesday"},{43,"every week"},{44,"everyday"},{45,"everyone"},{46,"everything"},{47,"f"},{48,"february"},{49,"few"},
  {50,"flat tire"},{51,"flat_tire"},{52,"friday"},{53,"g"},{54,"go"},{55,"good_afternoon"},{56,"good_evening"},{57,"good_morning"},{58,"h"},{59,"hail"},
  {60,"he"},{61,"heat wave"},{62,"hello"},{63,"her"},{64,"herself"},{65,"him"},{66,"himself"},{67,"hot"},{68,"hour"},{69,"how_are_you"},
  {70,"how_are_you_doing"},{71,"how_much_does_it..."},{72,"humid"},{73,"i"},{74,"i"},{75,"i_am_lost"},{76,"i_don't_know"},{77,"i_have_a_reservation"},{78,"ice"},{79,"internet"},
  {80,"is it safe"},{81,"is_it_close"},{82,"is_it_safe"},{83,"it"},{84,"itself"},{85,"j"},{86,"january"},{87,"july"},{88,"k"},{89,"l"},
  {90,"late night"},{91,"late"},{92,"lightning"},{93,"like"},{94,"lucky"},{95,"m"},{96,"many"},{97,"march"},{98,"maybe"},{99,"me"},
  {100,"midnight"},{101,"monday"},{102,"monthly"},{103,"morning dew"},{104,"morning"},{105,"my"},{106,"myself"},{107,"n"},{108,"next week"},{109,"night"},
  {110,"none"},{111,"noon"},{112,"nothing"},{113,"november"},{114,"o"},{115,"our"},{116,"ourselves"},{117,"p"},{118,"please_help_me"},{119,"pouring rain"},
  {120,"q"},{121,"r"},{122,"rainbow"},{123,"rainbow"},{124,"s"},{125,"scattered rain"},{126,"scattered snow"},{127,"second"},{128,"shocked"},{129,"signs"},
  {130,"sleep"},{131,"slippery walking"},{132,"slippery"},{133,"smog"},{134,"snow"},{135,"some"},{136,"somebody"},{137,"someone"},{138,"something"},{139,"soon"},
  {140,"spring"},{141,"stop"},{142,"summer"},{143,"sun"},{144,"sunday"},{145,"sunrise"},{146,"sunset"},{147,"t"},{148,"temperature"},{149,"thank_you"},
  {150,"their"},{151,"them"},{152,"thunder"},{153,"thursday"},{154,"time"},{155,"today"},{156,"traffic"},{157,"tuesday"},{158,"turn_around"},{159,"u"},
  {160,"u_turn"},{161,"us"},{162,"v"},{163,"vacation"},{164,"w"},{165,"water"},{166,"we"},{167,"weather"},{168,"wednesday"},{169,"weekend"},
  {170,"welcome"},{171,"what"},{172,"what's_up"},{173,"where_is_currency..."},{174,"where_is_phone"},{175,"which"},{176,"who"},{177,"windy"},{178,"wintering"},{179,"x"},
  {180,"y"},{181,"yourself"},{182,"z"}
};
int totalGestures = sizeof(gestures) / sizeof(gestures[0]);

void setup() {
  Serial.begin(115200);
  Wire.begin();

  if (!mpu.begin()) {
    Serial.println("MPU6050 not found!");
    while (1);
  }

  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("Connected!");
}

void loop() {
  // Read flex sensors
  for (int i = 0; i < 5; i++) {
    flexValues[i] = analogRead(flexPins[i]);
  }

  // Read gyro/accel
  mpu.update();
  int16_t ax = mpu.getAccX();
  int16_t ay = mpu.getAccY();
  int16_t az = mpu.getAccZ();
  int16_t gx = mpu.getGyroX();
  int16_t gy = mpu.getGyroY();
  int16_t gz = mpu.getGyroZ();

  // Recognize gesture
  String gesture = recognizeGesture(flexValues, ax, ay, az, gx, gy, gz);

  // Print on Serial Monitor
  Serial.println("Detected: " + gesture);

  // Send to website
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    http.begin(serverURL);
    http.addHeader("Content-Type", "application/json");
    String json = "{\"gesture\":\"" + gesture + "\"}";
    http.POST(json);
    http.end();
  }

  delay(500);
}

String recognizeGesture(int flexVals[5], int ax, int ay, int az, int gx, int gy, int gz) {
  // Rough threshold-based mapping for demo
  if (flexVals[0] > 2000 && ax > 500) return "a";
  if (flexVals[1] > 1800 && gy < -300) return "afternoon";
  if (flexVals[2] < 1200 && gz > 600) return "all";
  if (flexVals[3] > 1500 && ax < -500) return "annually";
  if (flexVals[4] > 1800 && ay > 400) return "any";

  // Add more gestures here with fine-tuned thresholds...

  return "unknown";
}
