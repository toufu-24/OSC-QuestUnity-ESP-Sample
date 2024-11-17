from pythonosc import dispatcher
from pythonosc import osc_server

# メッセージを処理する関数
def message_handler(address, *args):
    print(f"受信したアドレス: {address}")
    print(f"受信したデータ: {args}")

# ディスパッチャを作成してハンドラーを登録
dispatcher = dispatcher.Dispatcher()
dispatcher.map("/sample", message_handler)  # "/sample"アドレスに対応するハンドラーを登録

# サーバーを設定
ip = "127.0.0.1"  # 受信するIPアドレス
port = 8000       # 受信するポート番号

server = osc_server.BlockingOSCUDPServer((ip, port), dispatcher)

print(f"OSCサーバーが {ip}:{port} で開始されました...")
server.serve_forever()  # サーバーを起動
