using UnityEngine;
using UnityEngine.XR;
using OscJack;

// OSCメッセージを送信するクラス
public class SendMessage : MonoBehaviour
{
    [SerializeField] private string ipAddress = "ESP's ipaddress"; // OSCメッセージを受信するデバイスのIPアドレス
    [SerializeField] private int port = 8000; // 送信先のポート番号
    private OscClient _client; // OSC通信を行うためのクライアント
    private const XRNode ControllerNode = XRNode.RightHand; // XRコントローラ（右手デバイス）を指定

    // コンポーネントが有効化されたときに呼び出される
    private void OnEnable()
    {
        _client = new OscClient(ipAddress, port); // 指定したIPアドレスとポート番号でOSCクライアントを初期化
    }

    // コンポーネントが無効化されたときに呼び出される
    private void OnDisable()
    {
        _client.Dispose(); // OSCクライアントを解放
    }

    private void Update()
    {
        // Spaceキーが押された場合
        if (Input.GetKeyDown(KeyCode.Space))
        {
            // Debug.Log("Space key was pressed."); // ログを出力
            _client.Send("/sample", "Hello, I am Windows!"); // OSCでメッセージを送信
        }
        // コントローラのプライマリボタン（例: OculusのAボタン）が押された場合
        else if (InputDevices.GetDeviceAtXRNode(ControllerNode) // 指定されたXRデバイス（右手コントローラ）の状態を取得
                     .TryGetFeatureValue(CommonUsages.primaryButton, out bool primaryButtonValue) && primaryButtonValue)
        {
            _client.Send("/sample", "Hello, I am Quest3!"); // OSCでメッセージを送信
        }
    }
}