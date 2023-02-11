; Auto-generated. Do not edit!


(cl:in-package darknet-msg)


;//! \htmlinclude coordinate.msg.html

(cl:defclass <coordinate> (roslisp-msg-protocol:ros-message)
  ((x
    :reader x
    :initarg :x
    :type cl:fixnum
    :initform 0)
   (y
    :reader y
    :initarg :y
    :type cl:fixnum
    :initform 0))
)

(cl:defclass coordinate (<coordinate>)
  ())

(cl:defmethod cl:initialize-instance :after ((m <coordinate>) cl:&rest args)
  (cl:declare (cl:ignorable args))
  (cl:unless (cl:typep m 'coordinate)
    (roslisp-msg-protocol:msg-deprecation-warning "using old message class name darknet-msg:<coordinate> is deprecated: use darknet-msg:coordinate instead.")))

(cl:ensure-generic-function 'x-val :lambda-list '(m))
(cl:defmethod x-val ((m <coordinate>))
  (roslisp-msg-protocol:msg-deprecation-warning "Using old-style slot reader darknet-msg:x-val is deprecated.  Use darknet-msg:x instead.")
  (x m))

(cl:ensure-generic-function 'y-val :lambda-list '(m))
(cl:defmethod y-val ((m <coordinate>))
  (roslisp-msg-protocol:msg-deprecation-warning "Using old-style slot reader darknet-msg:y-val is deprecated.  Use darknet-msg:y instead.")
  (y m))
(cl:defmethod roslisp-msg-protocol:serialize ((msg <coordinate>) ostream)
  "Serializes a message object of type '<coordinate>"
  (cl:write-byte (cl:ldb (cl:byte 8 0) (cl:slot-value msg 'x)) ostream)
  (cl:write-byte (cl:ldb (cl:byte 8 8) (cl:slot-value msg 'x)) ostream)
  (cl:write-byte (cl:ldb (cl:byte 8 0) (cl:slot-value msg 'y)) ostream)
  (cl:write-byte (cl:ldb (cl:byte 8 8) (cl:slot-value msg 'y)) ostream)
)
(cl:defmethod roslisp-msg-protocol:deserialize ((msg <coordinate>) istream)
  "Deserializes a message object of type '<coordinate>"
    (cl:setf (cl:ldb (cl:byte 8 0) (cl:slot-value msg 'x)) (cl:read-byte istream))
    (cl:setf (cl:ldb (cl:byte 8 8) (cl:slot-value msg 'x)) (cl:read-byte istream))
    (cl:setf (cl:ldb (cl:byte 8 0) (cl:slot-value msg 'y)) (cl:read-byte istream))
    (cl:setf (cl:ldb (cl:byte 8 8) (cl:slot-value msg 'y)) (cl:read-byte istream))
  msg
)
(cl:defmethod roslisp-msg-protocol:ros-datatype ((msg (cl:eql '<coordinate>)))
  "Returns string type for a message object of type '<coordinate>"
  "darknet/coordinate")
(cl:defmethod roslisp-msg-protocol:ros-datatype ((msg (cl:eql 'coordinate)))
  "Returns string type for a message object of type 'coordinate"
  "darknet/coordinate")
(cl:defmethod roslisp-msg-protocol:md5sum ((type (cl:eql '<coordinate>)))
  "Returns md5sum for a message object of type '<coordinate>"
  "2b80853b9dd76da1c3efb4dbc2426fe9")
(cl:defmethod roslisp-msg-protocol:md5sum ((type (cl:eql 'coordinate)))
  "Returns md5sum for a message object of type 'coordinate"
  "2b80853b9dd76da1c3efb4dbc2426fe9")
(cl:defmethod roslisp-msg-protocol:message-definition ((type (cl:eql '<coordinate>)))
  "Returns full string definition for message of type '<coordinate>"
  (cl:format cl:nil "uint16 x~%~%uint16 y~%~%"))
(cl:defmethod roslisp-msg-protocol:message-definition ((type (cl:eql 'coordinate)))
  "Returns full string definition for message of type 'coordinate"
  (cl:format cl:nil "uint16 x~%~%uint16 y~%~%"))
(cl:defmethod roslisp-msg-protocol:serialization-length ((msg <coordinate>))
  (cl:+ 0
     2
     2
))
(cl:defmethod roslisp-msg-protocol:ros-message-to-list ((msg <coordinate>))
  "Converts a ROS message object to a list"
  (cl:list 'coordinate
    (cl:cons ':x (x msg))
    (cl:cons ':y (y msg))
))
