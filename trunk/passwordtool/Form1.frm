VERSION 5.00
Begin VB.Form Form1 
   BorderStyle     =   1  'Fixed Single
   Caption         =   "Password file creator"
   ClientHeight    =   5370
   ClientLeft      =   45
   ClientTop       =   330
   ClientWidth     =   10200
   LinkTopic       =   "Form1"
   MaxButton       =   0   'False
   MinButton       =   0   'False
   ScaleHeight     =   5370
   ScaleWidth      =   10200
   StartUpPosition =   3  'Windows Default
   Begin VB.CommandButton Command12 
      Caption         =   "Save"
      Height          =   495
      Left            =   6120
      TabIndex        =   12
      Top             =   4440
      Width           =   2895
   End
   Begin VB.CommandButton Command11 
      Caption         =   "Reset"
      Height          =   495
      Left            =   1200
      TabIndex        =   11
      Top             =   4440
      Width           =   2895
   End
   Begin VB.CommandButton Command10 
      Caption         =   "R Trigger"
      Height          =   495
      Left            =   8760
      TabIndex        =   10
      Top             =   1200
      Width           =   1215
   End
   Begin VB.CommandButton Command9 
      Caption         =   "L Trigger"
      Height          =   495
      Left            =   240
      TabIndex        =   9
      Top             =   1200
      Width           =   1215
   End
   Begin VB.CommandButton Command8 
      Caption         =   "Circle"
      Height          =   495
      Left            =   8160
      TabIndex        =   8
      Top             =   2400
      Width           =   1215
   End
   Begin VB.CommandButton Command7 
      Caption         =   "Square"
      Height          =   495
      Left            =   5760
      TabIndex        =   7
      Top             =   2400
      Width           =   1215
   End
   Begin VB.CommandButton Command6 
      Caption         =   "Cross"
      Height          =   495
      Left            =   6960
      TabIndex        =   6
      Top             =   3120
      Width           =   1215
   End
   Begin VB.CommandButton Command5 
      Caption         =   "Triangle"
      Height          =   495
      Left            =   6960
      TabIndex        =   5
      Top             =   1680
      Width           =   1215
   End
   Begin VB.CommandButton Command4 
      Caption         =   "Right"
      Height          =   495
      Left            =   3240
      TabIndex        =   4
      Top             =   2400
      Width           =   1215
   End
   Begin VB.CommandButton Command3 
      Caption         =   "Left"
      Height          =   495
      Left            =   840
      TabIndex        =   3
      Top             =   2400
      Width           =   1215
   End
   Begin VB.CommandButton Command2 
      Caption         =   "Down"
      Height          =   495
      Left            =   2040
      TabIndex        =   2
      Top             =   3120
      Width           =   1215
   End
   Begin VB.CommandButton Command1 
      Caption         =   "Up"
      Height          =   495
      Left            =   2040
      TabIndex        =   1
      Top             =   1680
      Width           =   1215
   End
   Begin VB.TextBox Text1 
      Height          =   615
      Left            =   120
      Locked          =   -1  'True
      MultiLine       =   -1  'True
      TabIndex        =   0
      Top             =   120
      Width           =   9975
   End
End
Attribute VB_Name = "Form1"
Attribute VB_GlobalNameSpace = False
Attribute VB_Creatable = False
Attribute VB_PredeclaredId = True
Attribute VB_Exposed = False
Private Buttons(0 To 15) As Byte
Private CountPos As Integer

Private Sub ClearButtons()

Dim i As Integer

For i = 0 To 15
    Buttons(i) = 0
Next i

Text1.Text = ""
CountPos = 0

End Sub

Private Sub Command1_Click()
    If CountPos < 10 Then
        Buttons(CountPos) = 5
        CountPos = CountPos + 1
        Text1.Text = Text1.Text & "Up "
    End If
End Sub

Private Sub Command10_Click()
    If CountPos < 10 Then
        Buttons(CountPos) = 10
        CountPos = CountPos + 1
        Text1.Text = Text1.Text & "R-Trigger "
    End If
End Sub

Private Sub Command11_Click()
    ClearButtons
End Sub

Private Sub Command12_Click()
On Error Resume Next
    Kill "buttons.ini"
    Open "buttons.ini" For Binary As #1
        Put #1, , Buttons()
    Close #1
    MsgBox "buttons.ini created!" & vbCrLf & "Put it in ms0:/seplugins/", vbOKOnly, "Success"
End Sub

Private Sub Command2_Click()
    If CountPos < 10 Then
        Buttons(CountPos) = 6
        CountPos = CountPos + 1
        Text1.Text = Text1.Text & "Down "
    End If
End Sub

Private Sub Command3_Click()
    If CountPos < 10 Then
        Buttons(CountPos) = 7
        CountPos = CountPos + 1
        Text1.Text = Text1.Text & "Left "
    End If
End Sub

Private Sub Command4_Click()
    If CountPos < 10 Then
        Buttons(CountPos) = 8
        CountPos = CountPos + 1
        Text1.Text = Text1.Text & "Right "
    End If
End Sub

Private Sub Command5_Click()
    If CountPos < 10 Then
        Buttons(CountPos) = 1
        CountPos = CountPos + 1
        Text1.Text = Text1.Text & "Triangle "
    End If
End Sub

Private Sub Command6_Click()
    If CountPos < 10 Then
        Buttons(CountPos) = 2
        CountPos = CountPos + 1
        Text1.Text = Text1.Text & "Cross "
    End If
End Sub

Private Sub Command7_Click()
    If CountPos < 10 Then
        Buttons(CountPos) = 3
        CountPos = CountPos + 1
        Text1.Text = Text1.Text & "Square "
    End If
End Sub

Private Sub Command8_Click()
    If CountPos < 10 Then
        Buttons(CountPos) = 4
        CountPos = CountPos + 1
        Text1.Text = Text1.Text & "Circle "
    End If
End Sub

Private Sub Command9_Click()
    If CountPos < 10 Then
        Buttons(CountPos) = 9
        CountPos = CountPos + 1
        Text1.Text = Text1.Text & "L-Trigger "
    End If
End Sub

Private Sub Form_Load()
    ClearButtons
End Sub
