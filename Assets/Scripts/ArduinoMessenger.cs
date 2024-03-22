using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class ArduinoMessenger : MonoBehaviour
{
    private SerialController _serialController;

    private MyListener _throttleHaver;
    private Rigidbody _rb;
    private int currentFrame = 0;
    [SerializeField] private int framesPerMessage = 60;

    // Start is called before the first frame update
    void Start()
    {
        _serialController = this.GetComponent<SerialController>();
        _throttleHaver = this.GetComponent<MyListener>();
        _rb = this.GetComponent<Rigidbody>();
    }

    // Update is called once per frame
    void Update()
    {
        if (currentFrame != framesPerMessage) { 
            // skip this frame's values
            currentFrame++;
            return;        
        }
        var dotProd = Vector3.Dot(Vector3.up, transform.TransformVector(Vector3.up));
        if (dotProd < 0f)
        {
            // if the plane is upside down, then we should flash the lights
            Debug.Log(
                $"1 {_throttleHaver.GetThrottle():F0} {transform.position.y:F0} {(_rb.velocity.magnitude * 3.6f):F0}\n");
            _serialController.SendSerialMessage(
                $"1\r{_throttleHaver.GetThrottle():F0}\r{transform.position.y:F0}\r{(_rb.velocity.magnitude * 3.6f):F0}");
        }
        else
        {
            Debug.Log(
                $"0\r{_throttleHaver.GetThrottle():F0}\r{transform.position.y:F0}\r{(_rb.velocity.magnitude * 3.6f):F0}");
            _serialController.SendSerialMessage(
                $"0\r{_throttleHaver.GetThrottle():F0}\r{transform.position.y:F0}\r{(_rb.velocity.magnitude * 3.6f):F0}");
        }

        currentFrame = 0;
    }
}
