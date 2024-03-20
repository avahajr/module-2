using System;
using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using TMPro;


public class MyListener : MonoBehaviour
{
    public float throttleIncrement = 0.1f;
    public float maxThrust = 200f;
    public float responsiveness = 10f;
    public float lift = 135f;

    private float throttle;
    private float roll;
    private float pitch;
    private float yaw;

    private Rigidbody rb;
    [SerializeField] private TextMeshProUGUI hud;
    [SerializeField] private Transform propeller;

    private float responseModifier
    {
        get { return (rb.mass / 10f) * responsiveness; }
    }

    // Start is called before the first frame update
    private void Awake()
    {
        rb = GetComponent<Rigidbody>();
    }

    private void HandleInputs()
    {
        // TODO: change later!
        roll = Input.GetAxis("Roll");
        pitch = Input.GetAxis("Pitch");
        yaw = Input.GetAxis("Yaw");

        if (Input.GetKey(KeyCode.Space))
        {
            throttle += throttleIncrement;
        }
        else
        {
            throttle -= throttleIncrement;
        }

        throttle = Mathf.Clamp(throttle, 0f, 100f);
    }

    void OnMessageArrived(string msg)
    {
        /*
         * Function is called each time Unity receives a message from Arduino.
         * msg: the message received.
         */

        string[] slicedMessage = msg.Split(' ');
        int[] serialIntegers = new int[slicedMessage.Length];

        for (int i = 0; i < slicedMessage.Length; i++)
        {
            serialIntegers[i] = int.Parse(slicedMessage[i]);
        }

        Debug.Log("X: " + serialIntegers[0] + " " + "Y: " + serialIntegers[1] + " " + "Z: " + serialIntegers[2] + " " +
                  "Potentiometer: " + serialIntegers[3]);

        yaw = serialIntegers[3] / (4095f/2f) - 1;
        pitch = serialIntegers[1] / (4095f/2f) - 1;
        roll = serialIntegers[0] / (4095f/2f) - 1;

        // Debug.Log("yaw: " + yaw);
        
        
    }

    void OnConnectionEvent(bool success)
    {
        Debug.Log(success ? "Device connected!" : "Device DISCONNECTED.");
    }

    // Update is called once per frame
    void Update()
    {
        // for a keyboard flight sim, uncomment the following line:
        // HandleInputs();

        UpdateHUD();
        if (throttle > 0)
        {
            propeller.Rotate(Vector3.right * throttle);
        }
    }

    private void FixedUpdate()
    {
        rb.AddForce(transform.forward * (maxThrust * throttle));
        rb.AddTorque(transform.up * (yaw * responseModifier));
        rb.AddTorque(transform.right * (roll * responseModifier));
        rb.AddTorque(-transform.forward * (pitch * responseModifier));

        rb.AddForce(Vector3.up * (rb.velocity.magnitude * lift));
    }

    private void UpdateHUD()
    {
        hud.text = "Throttle: " + throttle.ToString("F0") + "%\n";
        hud.text += "Airspeed: " + (rb.velocity.magnitude * 3.6f).ToString("F0") + "km/h\n";
        hud.text += "Altitude: " + transform.position.y.ToString("F0") + " m";
    }
}