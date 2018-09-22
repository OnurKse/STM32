Public Class frmUSB
    Dim led1 As Integer
    ' vendor and product IDs
    Private Const VendorID As Integer = &H483   'Replace with your device's
    Private Const ProductID As Integer = &H5750     'product and vendor IDs

    ' read and write buffers
    Private Const BufferInSize As Integer = 64 'Size of the data buffer coming IN to the PC
    Private Const BufferOutSize As Integer = 64    'Size of the data buffer going OUT from the PC
    Dim BufferIn(BufferInSize) As Byte          'Received data will be stored here - the first byte in the array is unused
    Dim BufferOut(BufferOutSize) As Byte    'Transmitted data is stored here - the first item in the array must be 0

    ' ****************************************************************
    ' when the form loads, connect to the HID controller - pass
    ' the form window handle so that you can receive notification
    ' events...
    '*****************************************************************
    Private Sub Form1_Load(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles MyBase.Load
        ConnectToHID(Me)
        led1 = 0
    End Sub

    '*****************************************************************
    ' disconnect from the HID controller...
    '*****************************************************************
    Private Sub Form1_FormClosed(ByVal sender As Object, ByVal e As System.Windows.Forms.FormClosedEventArgs) Handles Me.FormClosed
        DisconnectFromHID()
    End Sub

    '*****************************************************************
    ' a HID device has been plugged in...
    '*****************************************************************
    Public Sub OnPlugged(ByVal pHandle As Integer)
        If hidGetVendorID(pHandle) = VendorID And hidGetProductID(pHandle) = ProductID Then
            Label1.Text = "Baðlandý"
            Label2.Text = "VendorID: " + Hex(hidGetVendorID(pHandle))
            Label3.Text = "ProductID: " + Hex(hidGetProductID(pHandle))
            Label4.Text = "Aygýt: STM32F103C8T6"
            Label1.BackColor = Color.FromArgb(192, 255, 192)
        End If
    End Sub

    '*****************************************************************
    ' a HID device has been unplugged...
    '*****************************************************************
    Public Sub OnUnplugged(ByVal pHandle As Integer)
        If hidGetVendorID(pHandle) = VendorID And hidGetProductID(pHandle) = ProductID Then
            hidSetReadNotify(hidGetHandle(VendorID, ProductID), False)
            Label1.Text = "Baðlantý Bekleniyor..."
            Label2.Text = "VendorID: "
            Label3.Text = "ProductID: "
            Label4.Text = "Aygýt:"
            Label5.Text = "Veri:"
            Button1.BackgroundImage = My.Resources.off
            Label1.BackColor = Color.FromArgb(255, 192, 192)
        End If
    End Sub

    '*****************************************************************
    ' controller changed notification - called
    ' after ALL HID devices are plugged or unplugged
    '*****************************************************************
    Public Sub OnChanged()
        ' get the handle of the device we are interested in, then set
        ' its read notify flag to true - this ensures you get a read
        ' notification message when there is some data to read...
        Dim pHandle As Integer
        pHandle = hidGetHandle(VendorID, ProductID)
        hidSetReadNotify(hidGetHandle(VendorID, ProductID), True)
    End Sub

    '*****************************************************************
    ' on read event...
    '*****************************************************************
    Public Sub OnRead(ByVal pHandle As Integer)
        'read the data (don't forget, pass the whole array)...
        Dim i As Integer
        i = 1
        Label5.Text = "Veri: "
        Console.WriteLine(hidRead(pHandle, BufferIn(0)))
        If hidRead(pHandle, BufferIn(0)) Then
            While Not BufferIn(i) = 0
                Label5.Text += Chr(BufferIn(i))
                i = i + 1
            End While
            If Label5.Text = "Veri: off" Then
                Button1.BackgroundImage = My.Resources.off
            ElseIf Label5.Text = "Veri: on" Then
                Button1.BackgroundImage = My.Resources._on
            End If

            ' ** YOUR CODE HERE **
            ' first Byte Is the report ID, e.g.BufferIn(0)
            ' the other bytes are the data from the microcontroller...
        End If
    End Sub

    Private Sub Button1_Click(sender As Object, e As EventArgs) Handles Button1.Click
        If led1 = 0 Then
            BufferOut(1) = &H0
            'BufferOut(2) = &H10
            'BufferOut(4) = &H15
            led1 = 1
        ElseIf led1 = 1 Then
            BufferOut(1) = &H1
            'BufferOut(4) = &H89
            led1 = 0
        End If
        hidWriteEx(VendorID, ProductID, BufferOut(0))
        ' first Byte Is the report ID, e.g.BufferIn(0)
        ' the other bytes are the data from the microcontroller...
    End Sub

    Private Sub Timer1_Tick(sender As Object, e As EventArgs) Handles Timer1.Tick
        Label6.Text = "Saat: " + Date.Now.Hour.ToString + ":" + Date.Now.Minute.ToString + ":" + Date.Now.Second.ToString
    End Sub
End Class

