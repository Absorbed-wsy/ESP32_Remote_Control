import socket
import threading
import logging
import time
import os

# 全局变量，用于控制服务端和客户端的运行状态
udp_client_running = False
tcp_client_running = False

try:
    # 获取当前脚本的路径
    script_dir = os.path.dirname(os.path.abspath(__file__))
    
    # 拼接日志文件路径
    log_file_path = os.path.join(script_dir, 'python.log')

    # 配置日志记录
    logging.basicConfig(
        level=logging.INFO,  # 设置日志级别为 INFO
        format='%(asctime)s - %(levelname)s - %(message)s',  # 设置日志格式
        handlers=[
            logging.FileHandler(log_file_path),  # 将日志记录到文件
            logging.StreamHandler()  # 将日志输出到控制台
        ]
    )
    
    logging.info("APP Run!")
except PermissionError:
    logging.info("没有权限创建或写入日志文件。请检查目录权限。")
except Exception as e:
    logging.info(f"发生未知错误: {e}")
    
def control_menu():
    print("\n-------------------- Control Menu --------------------")
    print("map #Output pin diagram")
    print("------------------------------------------------------")
    print("gpio <init|deinit> <index> [in|out]")
    print("gpio <set|get> <index> [value]")
    print("------------------------------------------------------")
    print("i2c <init> <index> <scl_pin> <sda_pin>")
    print("i2c <deinit> <index>")
    print("i2c <set|get> <index> <daddr> <len> <value> [clk_freq]")
    print("------------------------------------------------------")
    print("spi <init|deinit> [clk_pin] [mosi_pin] [miso_pin]")
    print("spi <ts> <cs_pin> <mode> <rlen> <tlen> <value> [clk_freq]")
    print("------------------------------------------------------")
    print("adc <init|deinit> [channel]")
    print("adc <get>")
    print("------------------------------------------------------")
    print("pwm <init> <channel> <pin> <freq>")
    print("pwm <set> <channel> <duty>")
    print("------------------------------------------------------\n")
    
def esp32s3_map():
    print("\n---------- ESP32 S3 ----------")
    print("|GND  3V3 |        |IO1  GND |")
    print("|EN   IO4 |        |TXD0 IO2 |")
    print("|IO5  IO6 |        |IO42 RXD0|")
    print("|IO7  IO15|        |IO40 IO41|")
    print("|IO16 IO17|        |IO38 IO39|")
    print("|IO18 IO8 |        |IO36 IO37|")
    print("|IO19 IO20|        |IO0  IO35|")
    print("|GND IO9  IO11 IO13 IO21 IO48|")
    print("|5V  IO46 IO10 IO12 IO14 IO47|")
    print("|----------------------------|\n")
    print("ADC1 Channel: 0:IO1 1:IO2 2:IO3 3:IO4 4:IO5 5:IO6 6:IO7 7:IO8 8:IO9 9:IO10\n")
    
# UDP Client
def udp_client(server_ip, server_port):
    global udp_client_running
    client_socket = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    logging.info(f"UDP Client started, sending to {server_ip}:{server_port}")
    udp_client_running = True

    while udp_client_running:
        message = input("Enter message to send (or 'quit' to stop)> ")
        if message.lower() == "quit":
            break
        client_socket.sendto(message.encode(), (server_ip, server_port))
        data, addr = client_socket.recvfrom(1024)
        print(f"Received from server: {data.decode()}")

    client_socket.close()
    udp_client_running = False
    logging.info("UDP Client stopped")

# TCP Client
def tcp_client(server_ip, server_port):
    global tcp_client_running
    client_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    client_socket.connect((server_ip, server_port))
    client_socket.settimeout(5)
    logging.info(f"TCP Client started, connected to {server_ip}:{server_port}")
    tcp_client_running = True

    while tcp_client_running:
        message = input("Enter message to send (or 'help')> ")
        if message.lower() == "quit":
            break
        elif message.lower() == "help":
            control_menu()
        elif message.lower() == "map":
            esp32s3_map()
        else:
            client_socket.send(message.encode())
            data = client_socket.recv(1024)
            print(f"Received from server: {data.decode()}")

    client_socket.close()
    tcp_client_running = False
    logging.info("TCP Client stopped")

# 主菜单
def main_menu():
    print("\n--- Main Menu ---")
    print("1. Start UDP Client")
    print("2. Start TCP Client")
    print("3. Exit\n")
    print("port:8080 General command processing")
    print("port:8081 Camera data")
    choice = input("Enter your choice: ")
    return choice

# 主程序
if __name__ == "__main__":
    udp_client_thread = None
    tcp_client_thread = None
    
    while True:
        if not udp_client_running and not tcp_client_running:
            choice = main_menu()
            
        if choice == "1":
            if not udp_client_running:
                server_ip = input("Enter server IP: ")
                server_port = int(input("Enter server port: "))
                udp_client_thread = threading.Thread(target=udp_client, args=(server_ip, server_port))
                udp_client_thread.start()

        elif choice == "2":
            if not tcp_client_running:
                server_ip = input("Enter server IP: ")
                server_port = int(input("Enter server port: "))
                tcp_client_thread = threading.Thread(target=tcp_client, args=(server_ip, server_port))
                tcp_client_thread.start()

        elif choice == "3":
            logging.info("Exiting...")
            break

        else:
            logging.info("Invalid choice, please try again")
            
        choice == "0"

        time.sleep(1)
