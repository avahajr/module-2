using System;
using System.Collections;
using System.Collections.Generic;
using System.Numerics;
using UnityEngine;
using Vector3 = UnityEngine.Vector3;

public class CameraMovement : MonoBehaviour
{
    [SerializeField] private Transform[] povs;

    [SerializeField] private float speed;

    private int index = 1;

    private Vector3 target;
    
    // Update is called once per frame
    void Update()
    {
        if (Input.GetKeyDown("tab"))
        {
            index += 1;
        }

        if (index > 4)
        {
            index = 0;
        }

        target = povs[index].position;
    }

    private void FixedUpdate()
    {
        transform.position = Vector3.MoveTowards(transform.position, target, Time.deltaTime * speed);
        transform.forward = povs[index].forward;
        
    }
}
