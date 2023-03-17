import tkinter
import cv2
import PIL.Image, PIL.ImageTk
from stream import StreamCapture
import calibration
import requests

class App: 
    def __init__(self, window, window_title) -> None:
        self.img_cnt = 0
        self.window = window
        self.window.title(window_title)
        # self.video_source=video_source
        self.window.geometry("1280x780")
        self.matrix = []
        self.distCoeffs = []

        # self.window.configure(background="white")
        label1 = tkinter.Label(self.window, text = "Camera calibration", font=("Arial", 42))
        label2 = tkinter.Label(self.window, text = "Preview", font=("Arial", 28))
        label3 = tkinter.Label(self.window, text = "Needed amount of photos: 15", font=("Arial", 28))
        self.label4 = tkinter.Label(self.window, text = "Photos taken: " + str(self.img_cnt), font=("Arial", 28))

        self.stream = StreamCapture()
        self.canvas = tkinter.Canvas(self.window, width= self.stream.width, height=self.stream.height)

        self.buttonFrame = tkinter.Frame(self.window)
        btn1 = tkinter.Button(self.buttonFrame,width=40, height= 2, text="Take photo", command=self.capture_photo)
        btn2 = tkinter.Button(self.buttonFrame, width=40, height= 2, text="Count matrix", command=self.count_matrix)
        btn3 = tkinter.Button(self.buttonFrame, width=40, height= 2, text="Save matrix", command=self.save_matrix)


        label1.grid(row= 0, column=0, sticky=tkinter.W, padx= (20,0), pady=(20,0))
        label2.grid(row= 1, column=0, sticky=tkinter.W, padx= (20,0), pady=(20,0))
        label3.grid(row= 3, column=0, sticky=tkinter.W, padx= (20,0), pady=(20,0))
        self.label4.grid(row= 4, column=0, sticky=tkinter.W, padx= (20,0), pady=(20,0))
        self.canvas.grid(row= 2, column=0, sticky=tkinter.W, padx= (20,0), pady=(20,0))

        self.buttonFrame.grid(row=2, column=1)
        btn1.grid(row=0, column=0, sticky=tkinter.E, padx= (20,0), pady=(20,0))
        btn2.grid(row=1, column=0, sticky=tkinter.E, padx= (20,0), pady=(20,0))
        btn3.grid(row=2, column=0, sticky=tkinter.E, padx= (20,0), pady=(20,0))

        self.delay = 1
        self.update()    
        self.window.mainloop()
    
    def Close(self):
        self.window.destroy()

    def update(self):
        # Get a frame from the video source
        ret, frame = self.stream.get_frame()

        if ret:
            self.photo = PIL.ImageTk.PhotoImage(image = PIL.Image.fromarray(frame))
            self.canvas.create_image(0, 0, image = self.photo, anchor = tkinter.NW)

        self.window.after(self.delay, self.update)
    
    def capture_photo(self):
        ret, frame = self.stream.get_frame()

        if ret:
            cv2.imwrite("images/image" + str(self.img_cnt) + ".jpg",cv2.cvtColor(frame, cv2.COLOR_RGB2BGR))
            self.img_cnt+=1
            self.label4.config(text="Photos taken: " + str(self.img_cnt))
    
    def count_matrix(self):
        self.matrix, self.distCoeffs = calibration.countMatrix()
        print(self.matrix)
        print(self.distCoeffs)
    
    def save_matrix(self):
        del self.stream
        matrix_string = str(self.matrix[0,0]) + ";" + str(self.matrix[0,1]) + ";" + str(self.matrix[0,2]) + ";" + \
                        str(self.matrix[1,0]) + ";" + str(self.matrix[1,1]) + ";" + str(self.matrix[1,2]) + ";" + \
                        str(self.matrix[2,0]) + ";" + str(self.matrix[2,1]) + ";" + str(self.matrix[2,2]) + "|" + \
                        str(self.distCoeffs[0][0]) + ";" + str(self.distCoeffs[0][1]) + ";" + str(self.distCoeffs[0][2]) + ";" + \
                        str(self.distCoeffs[0][3]) + ";" + str(self.distCoeffs[0][4])
        
        try:
            requests.post("http://192.168.4.1", data=matrix_string, timeout=2.0)
        except requests.exceptions.Timeout:
            pass
        self.Close()
        

