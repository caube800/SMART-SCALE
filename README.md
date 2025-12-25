# ⚖️ Dự án Cân Điện Tử Thông Minh

Dự án này là hệ thống cân điện tử thông minh sử dụng **ESP32**, tích hợp ứng dụng **Blynk** để giám sát và điều khiển từ xa. Hệ thống có khả năng đo khối lượng, tự động tính tiền và in hóa đơn bán lẻ qua máy in nhiệt.

## 🌟 Tính năng chính

* **Đo khối lượng:** Sử dụng Load Cell + HX711, hiển thị đơn vị Gram (g) hoặc Kilogram (kg).
* **Tính tiền tự động:** Nhập đơn giá từ điện thoại, hệ thống tự động tính `Tổng tiền = Khối lượng x Đơn giá`.
* **In hóa đơn:** Kết nối máy in nhiệt (Thermal Printer) để in phiếu thanh toán gồm: Số hóa đơn, Khối lượng, Đơn giá, Tổng tiền.
* **Hiển thị:** Màn hình LCD 16x2 hiển thị trạng thái, khối lượng và giá tiền.
* **Điều khiển qua Blynk:**
    * Trừ bì (Tare).
    * Chuyển đổi chế độ hiển thị (Cân / Tính tiền).
    * Nhập và Reset đơn giá.
    * Ra lệnh in hóa đơn từ xa.

## 🛠️ Phần cứng yêu cầu

1.  **Vi điều khiển:** ESP32 Development Board (30 pin hoặc 38 pin).
2.  **Cảm biến:** Load Cell + Module HX711.
3.  **Hiển thị:** Màn hình LCD 16x2 (kết nối GPIO song song).
4.  **Máy in:** Máy in nhiệt (Thermal Printer) giao tiếp TTL Serial.
5.  **Khác:** Nguồn 5V DC (cấp cho ESP32 và máy in - khuyến nghị 2A trở lên).

## 🔌 Sơ đồ kết nối (Pinout)

Cấu hình chân dựa trên file `canthongminh.ino`:

| Thiết bị | Chân thiết bị | Chân ESP32 (GPIO) | Ghi chú |
| :--- | :--- | :--- | :--- |
| **HX711** | DOUT | 19 | Data Out |
| | CLK | 18 | Clock |
| **LCD 16x2** | RS | 32 | Register Select |
| | EN | 33 | Enable |
| | D4 | 25 | Data 4 |
| | D5 | 26 | Data 5 |
| | D6 | 27 | Data 6 |
| | D7 | 14 | Data 7 |
| **Máy in** | RX (Printer) | 17 (TX2) | Serial2 TX |
| | TX (Printer) | 16 (RX2) | Serial2 RX |

*(Lưu ý: Các chân VCC/GND nối với nguồn 5V/GND chung)*

## 📱 Cấu hình Blynk (Virtual Pins)

Tạo các Widget trên Blynk App và gán Virtual Pin như sau:

| Pin | Widget | Chức năng |
| :--- | :--- | :--- |
| **V0** | Button (Push) | **Tare**: Trừ bì (về 0). |
| **V1** | Button (Switch) | **Mode**: Chuyển hiển thị (0: Kg, 1: Giá tiền). |
| **V2** | Button (Push) | **Print**: In hóa đơn. |
| **V3** | Text Input | **Price Input**: Nhập đơn giá (VND/kg). |
| **V4** | Button (Push) | **Reset**: Xóa đơn giá và tổng tiền. |

## 📚 Thư viện cần cài đặt

Cài đặt các thư viện sau trong Arduino IDE:
1.  `HX711` (Bogdan Necula)
2.  `Blynk` (Volodymyr Shymanskyy)
3.  `Adafruit Thermal Printer Library` (Adafruit)
4.  `LiquidCrystal` (Built-in)

## ⚙️ Hướng dẫn cài đặt

1.  Mở file `canthongminh.ino`.
2.  Cập nhật thông tin cấu hình ở đầu file:
    ```cpp
    #define BLYNK_TEMPLATE_ID "TMPL..."      // Thay bằng ID của bạn
    #define BLYNK_TEMPLATE_NAME "canthongminh"
    #define BLYNK_AUTH_TOKEN "..."           // Thay bằng Token của bạn
    
    char ssid[] = "Ten_Wifi";                // Thay tên WiFi
    char pass[] = "Mat_Khau";                // Thay mật khẩu WiFi
    ```
3.  Kết nối phần cứng theo sơ đồ.
4.  Nạp code vào ESP32.
5.  Mở Serial Monitor (115200 baud) để kiểm tra kết nối.

## 📝 Lưu ý quan trọng

* **Hiệu chỉnh cân:** Biến `calibration_factor = 207146.47` trong code là giá trị mẫu. Bạn cần dùng vật mẫu (ví dụ quả cân 1kg) để tìm ra chỉ số chính xác cho Load Cell của mình.
* **Nguồn điện:** Máy in nhiệt tiêu thụ dòng lớn khi in, nếu ESP32 bị reset khi in, hãy dùng nguồn cấp riêng mạnh hơn cho máy in.