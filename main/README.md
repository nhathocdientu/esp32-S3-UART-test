# ESP32 UART Echo Example (ESP-IDF)

## UART Configuration

```c
#define UART_PORT      UART_NUM_0
#define BUF_SIZE       1024
```

| Macro | Meaning |
|---|---|
| `UART_PORT` | UART sử dụng (`UART0`) |
| `BUF_SIZE` | Kích thước buffer RX/TX |

---

# Log Tag

```c
static const char *TAG = "UART";
```

Dùng cho hệ thống log của ESP-IDF.

Ví dụ:

```txt
I (1234) UART: RX: abc
```

---

# app_main()

```c
void app_main(void)
```

Là hàm main của ESP-IDF.

ESP-IDF sẽ tự tạo FreeRTOS task chạy `app_main()`.

---

# uart_config_t

```c
uart_config_t uart_config =
{
    .baud_rate = 115200,
    .data_bits = UART_DATA_8_BITS,
    .parity    = UART_PARITY_DISABLE,
    .stop_bits = UART_STOP_BITS_1,
    .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
    .source_clk = UART_SCLK_DEFAULT
};
```

Struct cấu hình UART.

| Field | Meaning |
|---|---|
| `.baud_rate` | Baudrate UART |
| `.data_bits` | Số bit dữ liệu |
| `.parity` | Kiểm tra parity |
| `.stop_bits` | Stop bit |
| `.flow_ctrl` | Flow control |
| `.source_clk` | Clock source UART |

---

# ESP_ERROR_CHECK()

```c
ESP_ERROR_CHECK(...)
```

Macro kiểm tra lỗi trong ESP-IDF.

Nếu hàm trả về lỗi:

- in log lỗi
- dừng chương trình

---

# uart_param_config()

```c
uart_param_config(UART_PORT, &uart_config);
```

Cấu hình UART theo thông số trong `uart_config`.

---

# uart_driver_install()

```c
uart_driver_install(
    UART_PORT,
    BUF_SIZE,
    BUF_SIZE,
    0,
    NULL,
    0
);
```

Cài đặt UART driver.

## Parameters

| Parameter | Meaning |
|---|---|
| `UART_PORT` | UART sử dụng |
| `BUF_SIZE` | RX buffer size |
| `BUF_SIZE` | TX buffer size |
| `0` | Queue size |
| `NULL` | Queue handle |
| `0` | Interrupt flags |

ESP-IDF sẽ tạo:

- RX buffer
- TX buffer
- UART interrupt handler

---

# memset()

```c
memset(rx_data, 0, sizeof(rx_data));
```

Xóa dữ liệu cũ trong buffer bằng cách set toàn bộ buffer về `0`.

---

# uart_read_bytes()

```c
int len = uart_read_bytes(
    UART_PORT,
    rx_data,
    BUF_SIZE - 1,
    pdMS_TO_TICKS(1000)
);
```

Đọc dữ liệu từ UART.

## Parameters

| Parameter | Meaning |
|---|---|
| `UART_PORT` | UART sử dụng |
| `rx_data` | Buffer lưu dữ liệu |
| `BUF_SIZE - 1` | Số byte tối đa đọc |
| `pdMS_TO_TICKS(1000)` | Timeout 1000 ms |

## Return Value

```c
len
```

Số byte nhận được.

---

# pdMS_TO_TICKS()

```c
pdMS_TO_TICKS(1000)
```

Chuyển millisecond sang FreeRTOS tick.

Ví dụ:

```txt
1000 ms -> RTOS ticks
```

---

# rx_data[len] = '\0'

```c
rx_data[len] = '\0';
```

Thêm ký tự kết thúc chuỗi C (`NULL character`).

Giúp buffer trở thành string hợp lệ.

---

# strchr()

```c
p = strchr((char *)rx_data, '\n');
```

Tìm ký tự trong chuỗi.

## Example

Tìm:

```txt
'\n'
```

để xóa ký tự Enter.

---

# ESP_LOGI()

```c
ESP_LOGI(TAG, "RX: %s", rx_data);
```

In log ra terminal.

## Example Output

```txt
I (1234) UART: RX: abc
```

---

# snprintf()

```c
snprintf(
    tx_data,
    sizeof(tx_data),
    "Xin chao %.1000s\r\n",
    rx_data
);
```

Tạo chuỗi phản hồi an toàn.

## Example

Input:

```txt
abc
```

Output:

```txt
Xin chao abc
```

## Advantages

- tránh buffer overflow
- giới hạn độ dài chuỗi

---

# uart_write_bytes()

```c
uart_write_bytes(
    UART_PORT,
    tx_data,
    strlen(tx_data)
);
```

Gửi dữ liệu UART.

## Parameters

| Parameter | Meaning |
|---|---|
| `UART_PORT` | UART sử dụng |
| `tx_data` | Dữ liệu cần gửi |
| `strlen(tx_data)` | Số byte gửi |

---

# strlen()

```c
strlen(tx_data)
```

Tính độ dài chuỗi.

Ví dụ:

```txt
"abc"
```

=> length = 3

---

# vTaskDelay()

```c
vTaskDelay(pdMS_TO_TICKS(10));
```

Delay task trong FreeRTOS.

## Advantages

- không block toàn bộ CPU
- cho phép task khác chạy

---

# Program Flow

```txt
UART Receive
     ↓
uart_read_bytes()
     ↓
Remove '\r' '\n'
     ↓
Create response string
     ↓
uart_write_bytes()
     ↓
Send UART response
```