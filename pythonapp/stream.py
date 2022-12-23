import cv2

class StreamCapture:
    def __init__(self):
        self.stream = cv2.VideoCapture("http://192.168.4.1:80/stream")
        if not self.stream.isOpened():
            raise ValueError("Unable to capture the stream")
        
        self.width = self.stream.get(cv2.CAP_PROP_FRAME_WIDTH)
        self.height = self.stream.get(cv2.CAP_PROP_FRAME_HEIGHT)

    def get_frame(self):
        if self.stream.isOpened():
            ret, frame = self.stream.read()
            if ret:
                return (ret, cv2.cvtColor(frame, cv2.COLOR_BGR2RGB))
            else:
                return (ret, None)
        else:
            return (ret, None)
    
    def __del__(self):
        if self.stream.isOpened():
            self.stream.release()