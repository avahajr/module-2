using System;
using System.Collections;
using System.Collections.Generic;
using System.Text;
using UnityEngine;

public class ArduinoMessenger : MonoBehaviour
{
    private SerialController _serialController;

    private MyListener _throttleHaver;
    private Rigidbody _rb;
    private int _currentFrame;
    private char _lightState;
    private bool _isCloseToCrashing;
    
    // to not overwhelm the ESP32's serial queue, only transmit a message every framesPerMessage frames
    [SerializeField] private int framesPerMessage = 60;

    // Start is called before the first frame update
    void Start()
    {
        _serialController = this.GetComponent<SerialController>();
        _throttleHaver = this.GetComponent<MyListener>();
        _rb = this.GetComponent<Rigidbody>();
        _serialController.SetTearDownFunction(Teardown);
    }

    void FixedUpdate()
    {
        if (_currentFrame != framesPerMessage) { 
            // skip this frame's values
            _currentFrame++;
            return;        
        }

        _lightState = transform.position.y switch
        {
            // altitude-based lights
            > 200 when !_isCloseToCrashing => 'G',
            <= 200 when !_isCloseToCrashing => 'Y',
            _ => _lightState
        };

        if (!_isCloseToCrashing)
        {
            _serialController.SendSerialMessage(
                $" \r{_lightState}\r{_throttleHaver.GetThrottle():F0}\r{transform.position.y:F0}\r{(_rb.velocity.magnitude * 3.6f):F0}");
        }
        _currentFrame = 0;
    }

    private void OnTriggerEnter(Collider other)
    {
        Debug.Log("R");
        _isCloseToCrashing = true;
        // fast-track the serial message
        _serialController.SendSerialMessage(
            $" \rR\r{_throttleHaver.GetThrottle():F0}\r{transform.position.y:F0}\r{(_rb.velocity.magnitude * 3.6f):F0}");
    }

    private void OnTriggerExit(Collider other)
    {
        _isCloseToCrashing = false;
    }
    private void Teardown()
    {
        _serialController.SendSerialMessage(
            $"X\r{_lightState}\r{_throttleHaver.GetThrottle():F0}\r{transform.position.y:F0}\r{(_rb.velocity.magnitude * 3.6f):F0}");   
    }
}

