;; Auto-generated. Do not edit!


(when (boundp 'darknet::coordinate)
  (if (not (find-package "DARKNET"))
    (make-package "DARKNET"))
  (shadow 'coordinate (find-package "DARKNET")))
(unless (find-package "DARKNET::COORDINATE")
  (make-package "DARKNET::COORDINATE"))

(in-package "ROS")
;;//! \htmlinclude coordinate.msg.html


(defclass darknet::coordinate
  :super ros::object
  :slots (_x _y ))

(defmethod darknet::coordinate
  (:init
   (&key
    ((:x __x) 0)
    ((:y __y) 0)
    )
   (send-super :init)
   (setq _x (round __x))
   (setq _y (round __y))
   self)
  (:x
   (&optional __x)
   (if __x (setq _x __x)) _x)
  (:y
   (&optional __y)
   (if __y (setq _y __y)) _y)
  (:serialization-length
   ()
   (+
    ;; uint16 _x
    2
    ;; uint16 _y
    2
    ))
  (:serialize
   (&optional strm)
   (let ((s (if strm strm
              (make-string-output-stream (send self :serialization-length)))))
     ;; uint16 _x
       (write-word _x s)
     ;; uint16 _y
       (write-word _y s)
     ;;
     (if (null strm) (get-output-stream-string s))))
  (:deserialize
   (buf &optional (ptr- 0))
   ;; uint16 _x
     (setq _x (sys::peek buf ptr- :short)) (incf ptr- 2)
   ;; uint16 _y
     (setq _y (sys::peek buf ptr- :short)) (incf ptr- 2)
   ;;
   self)
  )

(setf (get darknet::coordinate :md5sum-) "2b80853b9dd76da1c3efb4dbc2426fe9")
(setf (get darknet::coordinate :datatype-) "darknet/coordinate")
(setf (get darknet::coordinate :definition-)
      "uint16 x

uint16 y
")



(provide :darknet/coordinate "2b80853b9dd76da1c3efb4dbc2426fe9")


