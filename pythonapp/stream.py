import cv2

class StreamCapture:
    def __init__(self):
        self.stream = cv2.VideoCapture("http://192.168.4.1:80/stream")
        # self.stream = cv2.VideoCapture(0)
        if not self.stream.isOpened():
            raise ValueError("Unable to capture the stream")
        
        self.width = 640
        self.height = 480

    def get_frame(self):
        if self.stream.isOpened():
            ret, frame = self.stream.read()
            if ret:
                frame = cv2.resize(frame, (640, 480))
                return (ret, cv2.cvtColor(frame, cv2.COLOR_BGR2RGB))
            else:
                return (ret, None)
        else:
            return (ret, None)
    
    def __del__(self):
        if self.stream.isOpened():
            self.stream.release()