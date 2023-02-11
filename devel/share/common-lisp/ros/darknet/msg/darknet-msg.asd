
(cl:in-package :asdf)

(defsystem "darknet-msg"
  :depends-on (:roslisp-msg-protocol :roslisp-utils )
  :components ((:file "_package")
    (:file "coordinate" :depends-on ("_package_coordinate"))
    (:file "_package_coordinate" :depends-on ("_package"))
  ))