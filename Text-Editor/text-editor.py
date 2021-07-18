from tkinter import *
from tkinter import filedialog
from tkinter import messagebox

import os
import filetype

# GUI Window built with Python
class V89TextEditor:
    savedFile = None
    unsavedFile = False
    userFileName = "Untitled"
    fileTypes = [("Text file", "*.txt")] #[('All types(*.*)', '*.*'), ('Text file', '*.txt')]

    def __init__(self):
        self.window = Tk()
        self.setWinTitle()

        #Menu
        self.menubar = Menu(self.window)
        self.filemenu = Menu(self.menubar, tearoff=0)

        self.menubar.add_cascade(label="File", underline=0, menu=self.filemenu)
        self.filemenu.add_command(label="New", underline=0, command=self.newTextFile)
        self.filemenu.add_command(label="Open", underline=0, command=self.openTextFile)
        self.filemenu.add_command(label="Save", underline=0, command=self.saveTextFile)
        #self.filemenu.add_command(label="Save as...", underline=1, command=self.donothing)
        #self.filemenu.add_command(label="Close", underline=0, command=self.newTextFile)

        self.filemenu.add_separator()

        self.filemenu.add_command(label="Exit", underline=1, command=self.exitNow)

        self.helpmenu = Menu(self.menubar, tearoff=0)
        self.menubar.add_cascade(label="Help", underline=0, menu=self.helpmenu)
        self.helpmenu.add_command(label="About...", underline=0, command=self.showAbout)

        # Elements
        self.textArea = Text(self.window, borderwidth=5, height=100, relief="sunken", width=300)

        # Scrollbars for text box
        self.scrollbar = Scrollbar(self.window)
        self.scrollbar.pack(side = RIGHT, fill = Y)
        self.textArea.yscrollcommand = self.scrollbar.set

        self.textArea.pack()
        self.textArea.bind("<Key>", self.addChangesToFile)
        self.textArea.focus_set()

        # Prevent the X button from automatically closing without performing the necessary checks
        self.window.protocol("WM_DELETE_WINDOW", self.exitNow)

        # Window settings
        self.window.geometry("800x600+0+0")

        self.window.config(menu=self.menubar)
        self.window.mainloop()

    def setWinTitle(self):
        unsavedFlag = "*" if self.unsavedFile == True else ""
        self.window.title(self.userFileName + str(unsavedFlag) + " :: Voyager 89 Text Editor")

    def newTextFile(self):
        if self.unsavedFile == True:
            if messagebox.askquestion("Question", "You're about to create a new text file, but you haven't saved your current text file.\n\nDo you wish to save it before you proceed?") == 'yes':
                self.saveTextFile()

        self.savedFile = None
        self.unsavedFile = False
        self.userFileName = "Untitled"
        self.textArea.delete(1.0, END)
        self.setWinTitle()

    def openTextFile(self):
        openedTextFile = filedialog.askopenfile(filetypes = self.fileTypes, defaultextension = self.fileTypes)

        if openedTextFile != None:
            # Check file type
            fileTypeData = filetype.guess(openedTextFile.name)

            if fileTypeData != None and fileTypeData.mime != "text/plain":
                messagebox.showerror("Error", "This is not a text file type: " + openedTextFile.name)
            else:
                # I don't want to bother with larger files yet..I suppose it's silly putting a cap, but I'll come back to this in version 2.0
                if os.path.getsize(openedTextFile.name) > 2000000 : # 2MB represented in bytes
                    messagebox.showinfo("File too big", "Apologies.\n\nThe maximum file size that can be read by this text editor is 2MB.")
                else:
                    self.unsavedFile = False
                    self.textArea.delete(1.0, END)
                    openedTextFileData = str(openedTextFile.name).split("/")
                    self.savedFile = openedTextFile
                    self.userFileName = openedTextFileData[len(openedTextFileData)-1]
                    with open(str(openedTextFile.name), "r") as readFile:
                        line = readFile.readline()

                        while line != "":
                            self.textArea.insert(END, line)
                            line = readFile.readline()

                    readFile.close()
                    self.setWinTitle()

    def exitNow(self):
        if self.unsavedFile == True:
            if messagebox.askquestion("Question", "You're about to exit, but you haven't saved your text file.\n\nDo you wish to save it before you quit?") == 'yes':
                self.saveTextFile(True)
            else:
                self.window.quit()
        else:
            self.window.quit()

    def addChangesToFile(self, event):
        #print("Key pressed: ", event)
        if (hasattr(event, 'char') and len(getattr(event, 'char')) == 1):
            #print("Char key: ", getattr(event, 'char'))
            self.unsavedFile = True
            self.setWinTitle()

    def saveTextFile(self, exitAfterSave=False):
        if self.userFileName.lower() == "untitled":
            self.savedFile = filedialog.asksaveasfile(filetypes = self.fileTypes, defaultextension = self.fileTypes)

        if self.savedFile != None:
            self.unsavedFile = False

            if self.userFileName.lower() == "untitled":
                savedData = str(self.savedFile.name).split("/")
                self.userFileName = savedData[len(savedData)-1]

            #print('Saved file-- ', self.userFileName)
            #self.savedFile.write(self.textArea.get(1.0, END))
            fileWrite = open(self.savedFile.name, "w")
            fileWrite.write(self.textArea.get(1.0, END))
            fileWrite.close()

            self.setWinTitle()

        if exitAfterSave == True:
            self.window.quit()

    def donothing(self):
        pass

    def showAbout(self):
        messagebox.showinfo("About Voyager 89 Text Editor", "Version 1.0 released on 20 March 2021\n---\nBuilt using Python.\n---\nwww.voyager89.net")


V89TextEditor() # Run the Voyager 89 Text Editor