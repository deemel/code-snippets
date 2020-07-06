Option Compare Database

Const MAX_SOLICITOR As Integer = 1
Const MAX_CUSIP As Integer = 10000
Const MAX_CLIENT As Integer = 10000
Const MAX_VOTING_DETAIL As Integer = 40
Const MAX_DIRECTOR_EXCEPTION_TOTAL As Integer = 40
Const MAX_TOTAL As Integer = 1

Const DATE_ZERO_STRING As String = "000000"
Const DATE_ZERO As Date = #1/1/100#

Const TABLE_SOLICITOR = "Solicitor"
Const TABLE_CUSIP = "CUSIP"
Const TABLE_CLIENT = "Client"
Const TABLE_VOTING_DETAIL = "VotingDetail"
'Const TABLE_VOTING_DETAIL_INITIAL = "VotingDetailInitial"
'Const TABLE_VOTING_DETAIL_REVISED = "VotingDetailRevised"
Const TABLE_DIRECTOR_EXCEPTION_TOTAL = "DirectorExceptionTotal"
Const TABLE_TOTAL = "Total"

Const FIELD_VOTING_DETAIL_FOR_VOTES = "ForVotes"
Const FIELD_VOTING_DETAIL_AGAINST_VOTES = "AgainstVotes"
Const FIELD_VOTING_DETAIL_ABSTAIN_VOTES = "AbstainVotes"
Const FIELD_DIRECTOR_EXCEPTION_TOTAL_NAME = "Name"
Const FIELD_DIRECTOR_EXCEPTION_TOTAL_SHARES_WITHHELD = "SharesWithheld"

Type TypeVotingDetail
  IdVotingDetail As Variant
  IdClient As Variant

  RecordId As Integer
  ProposalRecordNumber As Integer
  NotUsed1 As String
  ClientNumber As String
  NotUsed2 As String
  SharesHeld As Variant
  DecimalIndicator As Integer
  ForVotes As Variant
  AgainstVotes As Variant
  AbstainVotes As Variant
  NotUsed3 As String
End Type

Type TypeDirectorExceptionTotal
  IdDirectorExceptionTotal As Variant
  IdClient As Variant

  RecordId As Integer
  Number As Integer
  NotUsed1 As String
  Name As String
  ItemCount As Long
  SharesWithheld As Variant
  NotUsed2 As String
End Type

Type TypeClient
  IdClient As Variant
  IdCUSIP As Variant

  RecordId As Integer
  NotUsed1 As String
  Number As String
  NotUsed2 As String
  Name As String
  Nominee As String
  DTCNumber As String
  NotUsed3 As String

  NumVotingDetailInitial As Integer
  NumVotingDetailRevised As Integer
  NumDirectorExceptionTotal As Integer
  VotingDetailInitial() As TypeVotingDetail
  VotingDetailRevised() As TypeVotingDetail
  DirectorExceptionTotal() As TypeDirectorExceptionTotal
End Type

Type TypeCUSIP
  IdCUSIP As Variant
  IdSolicitor As Variant

  RecordId As Integer
  NotUsed1 As String
  Number As String
  TypeOfMailing As String
  Name As String
  NotUsed2 As String
  RecordDate As Date
  MeetingDate As Date
  MailDate As Date
  PreviousVoteDate As Date
  NotUsed3 As String

  NumClient As Integer
  Client() As TypeClient
End Type

Type TypeTotal
  IdTotal As Variant
  IdSolicitor As Variant

  RecordId As Integer
  NotUsed1 As String
  RecordCount As Integer
  NotUsed2 As String
End Type

Type TypeSolicitor
  IdSolicitor As Variant

  RecordId As Integer
  Number As String
  NotUsed1 As String
  Name As String
  TabulationDate As Date
  FileId As String
  NotUsed2 As String
  TransmissionControlNumber As Integer
  NotUsed3 As String

  NumCUSIP As Integer
  NumTotal As Integer
  CUSIP() As TypeCUSIP
  Total() As TypeTotal
End Type

Type TypeProxyTransmission
  NumSolicitor As Integer
  Solicitor() As TypeSolicitor
End Type

Dim ProxyTransmission As TypeProxyTransmission

Function SetSolicitor(RecordId As Integer, InputBuffer As String) As TypeSolicitor
  Dim Solicitor As TypeSolicitor

  With Solicitor
    .RecordId = RecordId
    .Number = RTrim(Mid(InputBuffer, 4, 6))
    .NotUsed1 = RTrim(Mid(InputBuffer, 10, 5))
    .Name = RTrim(Mid(InputBuffer, 15, 40))
    .TabulationDate = DateSerial(Mid(InputBuffer, 55, 2), Mid(InputBuffer, 57, 2), Mid(InputBuffer, 59, 2))
    .FileId = RTrim(Mid(InputBuffer, 61, 6))
    .NotUsed2 = RTrim(Mid(InputBuffer, 67, 3))
    .TransmissionControlNumber = Mid(InputBuffer, 70, 7)
    .NotUsed3 = RTrim(Mid(InputBuffer, 77, 4))

    .NumCUSIP = 0
    .NumTotal = 0
  End With

  SetSolicitor = Solicitor
End Function

Function SetCUSIP(RecordId As Integer, InputBuffer As String) As TypeCUSIP
  Dim CUSIP As TypeCUSIP

  With CUSIP
    .RecordId = RecordId
    .NotUsed1 = RTrim(Mid(InputBuffer, 4, 2))
    .Number = RTrim(Mid(InputBuffer, 6, 9))
    .TypeOfMailing = RTrim(Mid(InputBuffer, 15, 1))
    .Name = RTrim(Mid(InputBuffer, 16, 33))
    .NotUsed2 = RTrim(Mid(InputBuffer, 49, 7))
    .RecordDate = DateSerial(Mid(InputBuffer, 56, 2), Mid(InputBuffer, 58, 2), Mid(InputBuffer, 60, 2))
    .MeetingDate = DateSerial(Mid(InputBuffer, 62, 2), Mid(InputBuffer, 64, 2), Mid(InputBuffer, 66, 2))
    .MailDate = DateSerial(Mid(InputBuffer, 68, 2), Mid(InputBuffer, 70, 2), Mid(InputBuffer, 72, 2))
    If Mid(InputBuffer, 74, 6) <> DATE_ZERO_STRING Then
      .PreviousVoteDate = DateSerial(Mid(InputBuffer, 74, 2), Mid(InputBuffer, 76, 2), Mid(InputBuffer, 78, 2))
    Else
      .PreviousVoteDate = DATE_ZERO
    End If
    .NotUsed3 = RTrim(Mid(InputBuffer, 80, 1))

    .NumClient = 0
  End With

  SetCUSIP = CUSIP
End Function

Function SetClient(RecordId As Integer, InputBuffer As String) As TypeClient
  Dim Client As TypeClient

  With Client
    .RecordId = RecordId
    .NotUsed1 = RTrim(Mid(InputBuffer, 4, 2))
    .Number = RTrim(Mid(InputBuffer, 6, 3))
    .NotUsed2 = RTrim(Mid(InputBuffer, 9, 10))
    .Name = RTrim(Mid(InputBuffer, 19, 20))
    .Nominee = RTrim(Mid(InputBuffer, 39, 34))
    .DTCNumber = Mid(InputBuffer, 73, 5)
    .NotUsed3 = RTrim(Mid(InputBuffer, 78, 3))

    .NumVotingDetailInitial = 0
    .NumVotingDetailRevised = 0
    .NumDirectorExceptionTotal = 0
  End With

  SetClient = Client
End Function

Function NumberAscii(NumberEbcdic As String) As Variant
  Dim TrailingSign As String

  TrailingSign = Right(NumberEbcdic, 1)
  Select Case TrailingSign
    Case "0" To "9"
      NumberAscii = CDec(NumberEbcdic)
    Case "{"
      NumberAscii = CDec("+" & Mid(NumberEbcdic, 1, 14) & "0")
    Case "A"
      NumberAscii = CDec("+" & Mid(NumberEbcdic, 1, 14) & "1")
    Case "B"
      NumberAscii = CDec("+" & Mid(NumberEbcdic, 1, 14) & "2")
    Case "C"
      NumberAscii = CDec("+" & Mid(NumberEbcdic, 1, 14) & "3")
    Case "D"
      NumberAscii = CDec("+" & Mid(NumberEbcdic, 1, 14) & "4")
    Case "E"
      NumberAscii = CDec("+" & Mid(NumberEbcdic, 1, 14) & "5")
    Case "F"
      NumberAscii = CDec("+" & Mid(NumberEbcdic, 1, 14) & "6")
    Case "G"
      NumberAscii = CDec("+" & Mid(NumberEbcdic, 1, 14) & "7")
    Case "H"
      NumberAscii = CDec("+" & Mid(NumberEbcdic, 1, 14) & "8")
    Case "I"
      NumberAscii = CDec("+" & Mid(NumberEbcdic, 1, 14) & "9")
    Case "}"
      NumberAscii = CDec("-" & Mid(NumberEbcdic, 1, 14) & "0")
    Case "J"
      NumberAscii = CDec("-" & Mid(NumberEbcdic, 1, 14) & "1")
    Case "K"
      NumberAscii = CDec("-" & Mid(NumberEbcdic, 1, 14) & "2")
    Case "L"
      NumberAscii = CDec("-" & Mid(NumberEbcdic, 1, 14) & "3")
    Case "M"
      NumberAscii = CDec("-" & Mid(NumberEbcdic, 1, 14) & "4")
    Case "N"
      NumberAscii = CDec("-" & Mid(NumberEbcdic, 1, 14) & "5")
    Case "O"
      NumberAscii = CDec("-" & Mid(NumberEbcdic, 1, 14) & "6")
    Case "P"
      NumberAscii = CDec("-" & Mid(NumberEbcdic, 1, 14) & "7")
    Case "Q"
      NumberAscii = CDec("-" & Mid(NumberEbcdic, 1, 14) & "8")
    Case "R"
      NumberAscii = CDec("-" & Mid(NumberEbcdic, 1, 14) & "9")
    Case Else
      Debug.Print "Unrecognized trailing sign " & TrailingSign & " in EBCDIC number " & NumberEbcdic & ".  Setting trailing sign to 0."

      NumberAscii = CDec("+" & Mid(NumberEbcdic, 1, 14) & "0")
  End Select
End Function

Function SetVotingDetail(InputBuffer As String) As TypeVotingDetail
  Dim VotingDetail As TypeVotingDetail

  With VotingDetail
    .RecordId = Mid(InputBuffer, 1, 1)
    .ProposalRecordNumber = Mid(InputBuffer, 2, 2)
    .NotUsed1 = RTrim(Mid(InputBuffer, 4, 2))
    .ClientNumber = RTrim(Mid(InputBuffer, 6, 3))
    .NotUsed2 = RTrim(Mid(InputBuffer, 9, 10))
    .SharesHeld = NumberAscii(Mid(InputBuffer, 19, 15))
    .DecimalIndicator = Mid(InputBuffer, 34, 1)
    .ForVotes = NumberAscii(Mid(InputBuffer, 35, 15))
    .AgainstVotes = NumberAscii(Mid(InputBuffer, 50, 15))
    .AbstainVotes = NumberAscii(Mid(InputBuffer, 65, 15))
    .NotUsed3 = RTrim(Mid(InputBuffer, 80, 1))
  End With

  SetVotingDetail = VotingDetail
End Function

Function SetDirectorExceptionTotal(InputBuffer As String) As TypeDirectorExceptionTotal
  Dim DirectorExceptionTotal As TypeDirectorExceptionTotal

  With DirectorExceptionTotal
    .RecordId = Mid(InputBuffer, 1, 1)
    .Number = Mid(InputBuffer, 2, 2)
    .NotUsed1 = RTrim(Mid(InputBuffer, 4, 3))
    .Name = RTrim(Mid(InputBuffer, 7, 20))
    .ItemCount = Mid(InputBuffer, 27, 9)
    .SharesWithheld = NumberAscii(Mid(InputBuffer, 36, 15))
    .NotUsed2 = RTrim(Mid(InputBuffer, 51, 30))
  End With

  SetDirectorExceptionTotal = DirectorExceptionTotal
End Function

Function SetTotal(RecordId As Integer, InputBuffer As String) As TypeTotal
  Dim Total As TypeTotal

  With Total
    .RecordId = RecordId
    .NotUsed1 = RTrim(Mid(InputBuffer, 4, 6))
    .RecordCount = Mid(InputBuffer, 10, 9)
    .NotUsed2 = RTrim(Mid(InputBuffer, 19, 62))
  End With

  SetTotal = Total
End Function

Function Bulkload(Filename As String) As Boolean
  Dim InputBuffer As String
  Dim RecordId As Integer
  Dim LineNumber As Variant
  Dim FlagCUSIP As Boolean
  Dim FlagClient As Boolean

  Bulkload = False
  Open Filename For Input As #1

  FlagCUSIP = False
  FlagClient = False
  LineNumber = 1
  ProxyTransmission.NumSolicitor = 0

  Do While Not EOF(1)
    Line Input #1, InputBuffer

    RecordId = Mid(InputBuffer, 1, 3)
    Select Case RecordId
      Case 100
        With ProxyTransmission
          .NumSolicitor = .NumSolicitor + 1

          If .NumSolicitor > MAX_SOLICITOR Then
            Debug.Print InputBuffer
            Debug.Print "Solicitor record ID " & RecordId & " in line " & LineNumber & " exceeds maximum occurrences " & MAX_SOLICITOR & ".  Exiting."

            Close #1
            Exit Function
          End If

          If .NumSolicitor <= 1 Then
            ReDim .Solicitor(MAX_SOLICITOR)
          End If

          .Solicitor(.NumSolicitor) = SetSolicitor(RecordId, InputBuffer)
        End With

        FlagCUSIP = False
        FlagClient = False
      Case 200
        With ProxyTransmission

          If .NumSolicitor <= 0 Then
            Debug.Print InputBuffer
            Debug.Print "No solicitor record found prior to CUSIP record ID " & RecordId & " in line " & LineNumber & ".  Exiting."

            Close #1
            Exit Function
          End If

          With .Solicitor(.NumSolicitor)
            .NumCUSIP = .NumCUSIP + 1

            If .NumCUSIP > MAX_CUSIP Then
              Debug.Print InputBuffer
              Debug.Print "CUSIP record ID " & RecordId & " in line " & LineNumber & " exceeds maximum occurrences " & MAX_CUSIP & ".  Exiting."

              Close #1
              Exit Function
            End If

            If .NumCUSIP <= 1 Then
              ReDim .CUSIP(MAX_CUSIP)
            End If

            .CUSIP(.NumCUSIP) = SetCUSIP(RecordId, InputBuffer)
          End With
        End With

        FlagCUSIP = True
        FlagClient = False
      Case 250
        With ProxyTransmission

          If .NumSolicitor <= 0 Then
            Debug.Print InputBuffer
            Debug.Print "No solicitor record found prior to client record ID " & RecordId & " in line " & LineNumber & ".  Exiting."

            Close #1
            Exit Function
          End If

          If Not FlagCUSIP Then
            Debug.Print InputBuffer
            Debug.Print "No CUSIP record found prior to client record ID " & RecordId & " in line " & LineNumber & ".  Exiting."

            Close #1
            Exit Function
          End If

          With .Solicitor(.NumSolicitor)
            With .CUSIP(.NumCUSIP)
              .NumClient = .NumClient + 1

              If .NumClient > MAX_CLIENT Then
                Debug.Print InputBuffer
                Debug.Print "Client record ID " & RecordId & " in line " & LineNumber & " exceeds maximum occurrences " & MAX_CLIENT & ".  Exiting."

                Close #1
                Exit Function
              End If

              If .NumClient <= 1 Then
                ReDim .Client(MAX_CLIENT)
              End If

              .Client(.NumClient) = SetClient(RecordId, InputBuffer)
            End With
          End With
        End With

        FlagClient = True
      Case 301 To 320
        With ProxyTransmission

          If .NumSolicitor <= 0 Then
            Debug.Print InputBuffer
            Debug.Print "No solicitor record found prior to initial voting record ID " & RecordId & " in line " & LineNumber & ".  Exiting."

            Close #1
            Exit Function
          End If

          If Not FlagCUSIP Then
            Debug.Print InputBuffer
            Debug.Print "No CUSIP record found prior to initial voting record ID " & RecordId & " in line " & LineNumber & ".  Exiting."

            Close #1
            Exit Function
          End If

          If Not FlagClient Then
            Debug.Print InputBuffer
            Debug.Print "No client record found prior to initial voting record ID " & RecordId & " in line " & LineNumber & ".  Exiting."

            Close #1
            Exit Function
          End If

          With .Solicitor(.NumSolicitor)
            With .CUSIP(.NumCUSIP)
              With .Client(.NumClient)
                .NumVotingDetailInitial = .NumVotingDetailInitial + 1

                If .NumVotingDetailInitial > MAX_VOTING_DETAIL Then
                  Debug.Print InputBuffer
                  Debug.Print "Initial voting detail record ID " & RecordId & " in line " & LineNumber & " exceeds maximum occurrences " & MAX_VOTING_DETAIL & ".  Exiting."

                  Close #1
                  Exit Function
                End If

                If .NumVotingDetailInitial <= 1 Then
                  ReDim .VotingDetailInitial(MAX_VOTING_DETAIL)
                End If

                .VotingDetailInitial(.NumVotingDetailInitial) = SetVotingDetail(InputBuffer)
              End With
            End With
          End With
        End With
      Case 401 To 420
        With ProxyTransmission

          If .NumSolicitor <= 0 Then
            Debug.Print InputBuffer
            Debug.Print "No solicitor record found prior to revised voting record ID " & RecordId & " in line " & LineNumber & ".  Exiting."

            Close #1
            Exit Function
          End If

          If Not FlagCUSIP Then
            Debug.Print InputBuffer
            Debug.Print "No CUSIP record found prior to revised voting record ID " & RecordId & " in line " & LineNumber & ".  Exiting."

            Close #1
            Exit Function
          End If

          If Not FlagClient Then
            Debug.Print InputBuffer
            Debug.Print "No client record found prior to revised voting record ID " & RecordId & " in line " & LineNumber & ".  Exiting."

            Close #1
            Exit Function
          End If

          With .Solicitor(.NumSolicitor)
            With .CUSIP(.NumCUSIP)
              With .Client(.NumClient)
                .NumVotingDetailRevised = .NumVotingDetailRevised + 1

                If .NumVotingDetailRevised > MAX_VOTING_DETAIL Then
                  Debug.Print InputBuffer
                  Debug.Print "Revised voting detail record ID " & RecordId & " in line " & LineNumber & " exceeds maximum occurrences " & MAX_VOTING_DETAIL & ".  Exiting."

                  Close #1
                  Exit Function
                End If

                If .NumVotingDetailRevised <= 1 Then
                  ReDim .VotingDetailRevised(MAX_VOTING_DETAIL)
                End If

                .VotingDetailRevised(.NumVotingDetailRevised) = SetVotingDetail(InputBuffer)
              End With
            End With
          End With
        End With
      Case 501 To 540
        With ProxyTransmission

          If .NumSolicitor <= 0 Then
            Debug.Print InputBuffer
            Debug.Print "No solicitor record found prior to director exception total record ID " & RecordId & " in line " & LineNumber & ".  Exiting."

            Close #1
            Exit Function
          End If

          If Not FlagCUSIP Then
            Debug.Print InputBuffer
            Debug.Print "No CUSIP record found prior to director exception total record ID " & RecordId & " in line " & LineNumber & ".  Exiting."

            Close #1
            Exit Function
          End If

          If Not FlagClient Then
            Debug.Print InputBuffer
            Debug.Print "No client record found prior to director exception total record ID " & RecordId & " in line " & LineNumber & ".  Exiting."

            Close #1
            Exit Function
          End If

          With .Solicitor(.NumSolicitor)
            With .CUSIP(.NumCUSIP)
              With .Client(.NumClient)
                .NumDirectorExceptionTotal = .NumDirectorExceptionTotal + 1

                If .NumDirectorExceptionTotal > MAX_DIRECTOR_EXCEPTION_TOTAL Then
                  Debug.Print InputBuffer
                  Debug.Print "Director exception total record ID " & RecordId & " in line " & LineNumber & " exceeds maximum occurrences " & MAX_DIRECTOR_EXCEPTION_TOTAL & ".  Exiting."

                  Close #1
                  Exit Function
                End If

                If .NumDirectorExceptionTotal <= 1 Then
                  ReDim .DirectorExceptionTotal(MAX_DIRECTOR_EXCEPTION_TOTAL)
                End If

                .DirectorExceptionTotal(.NumDirectorExceptionTotal) = SetDirectorExceptionTotal(InputBuffer)
              End With
            End With
          End With
        End With
      Case 900
        With ProxyTransmission

          If .NumSolicitor <= 0 Then
            Debug.Print InputBuffer
            Debug.Print "No solicitor record found prior to total record ID " & RecordId & " in line " & LineNumber & ".  Exiting."

            Close #1
            Exit Function
          End If

          With .Solicitor(.NumSolicitor)
            .NumTotal = .NumTotal + 1

            If .NumTotal > MAX_TOTAL Then
              Debug.Print InputBuffer
              Debug.Print "Total record ID " & RecordId & " in line " & LineNumber & " exceeds maximum occurrences " & MAX_TOTAL & ".  Exiting."

              Close #1
              Exit Function
            End If

            If .NumTotal <= 1 Then
              ReDim .Total(MAX_TOTAL)
            End If

            .Total(.NumTotal) = SetTotal(RecordId, InputBuffer)
          End With
        End With

        FlagCUSIP = False
        FlagClient = False
      Case Else
        Debug.Print InputBuffer
        Debug.Print "Unrecognized record ID " & RecordId & " in line " & LineNumber & ".  Exiting."

        Close #1
        Exit Function
    End Select

    LineNumber = LineNumber + 1
  Loop

  Close #1

  'Bulkload

  Dim Field As String
  Dim FlagDoneOnce As Boolean

  Dim DB As Database
  Dim RecSolicitor As Recordset
  Dim RecCUSIP As Recordset
  Dim RecClient As Recordset
  Dim RecVotingDetail As Recordset
'  Dim RecVotingDetailInitial As Recordset
'  Dim RecVotingDetailRevised As Recordset
  Dim RecDirectorExceptionTotal As Recordset
  Dim RecTotal As Recordset

  Set DB = CurrentDb()
  Set RecSolicitor = DB.OpenRecordset(TABLE_SOLICITOR)
  Set RecCUSIP = DB.OpenRecordset(TABLE_CUSIP)
  Set RecClient = DB.OpenRecordset(TABLE_CLIENT)
  Set RecVotingDetail = DB.OpenRecordset(TABLE_VOTING_DETAIL)
'  Set RecVotingDetailInitial = DB.OpenRecordset(TABLE_VOTING_DETAIL_INITIAL)
'  Set RecVotingDetailRevised = DB.OpenRecordset(TABLE_VOTING_DETAIL_REVISED)
  Set RecDirectorExceptionTotal = DB.OpenRecordset(TABLE_DIRECTOR_EXCEPTION_TOTAL)
  Set RecTotal = DB.OpenRecordset(TABLE_TOTAL)

  With ProxyTransmission
    For i = 1 To .NumSolicitor
      With .Solicitor(i)
        RecSolicitor.AddNew

        RecSolicitor!RecordId = .RecordId
        RecSolicitor!Number = .Number
        RecSolicitor!NotUsed1 = .NotUsed1
        RecSolicitor!Name = .Name
        RecSolicitor!TabulationDate = .TabulationDate
        RecSolicitor!FileId = .FileId
        RecSolicitor!NotUsed2 = .NotUsed2
        RecSolicitor!TransmissionControlNumber = .TransmissionControlNumber
        RecSolicitor!NotUsed3 = .NotUsed3

        RecSolicitor.Update
        RecSolicitor.MoveLast

        .IdSolicitor = RecSolicitor!IdSolicitor

        For j = 1 To .NumCUSIP
          With .CUSIP(j)
            .IdSolicitor = RecSolicitor!IdSolicitor

            RecCUSIP.AddNew

            RecCUSIP!IdSolicitor = .IdSolicitor
            RecCUSIP!RecordId = .RecordId
            RecCUSIP!NotUsed1 = .NotUsed1
            RecCUSIP!Number = .Number
            RecCUSIP!TypeOfMailing = .TypeOfMailing
            RecCUSIP!Name = .Name
            RecCUSIP!NotUsed2 = .NotUsed2
            RecCUSIP!RecordDate = .RecordDate
            RecCUSIP!MeetingDate = .MeetingDate
            RecCUSIP!MailDate = .MailDate
            If .PreviousVoteDate <> DATE_ZERO Then
              RecCUSIP!PreviousVoteDate = .PreviousVoteDate
            End If
            RecCUSIP!NotUsed3 = .NotUsed3

            RecCUSIP.Update
            RecCUSIP.MoveLast

            .IdCUSIP = RecCUSIP!IdCUSIP

            For k = 1 To .NumClient
              With .Client(k)
                .IdCUSIP = RecCUSIP!IdCUSIP

                RecClient.AddNew

                RecClient!IdCUSIP = .IdCUSIP
                RecClient!RecordId = .RecordId
                RecClient!NotUsed1 = .NotUsed1
                RecClient!Number = .Number
                RecClient!NotUsed2 = .NotUsed2
                RecClient!Name = .Name
                RecClient!Nominee = .Nominee
                RecClient!DTCNumber = .DTCNumber
                RecClient!NotUsed3 = .NotUsed3

                RecClient.Update
                RecClient.MoveLast

                .IdClient = RecClient!IdClient

                FlagDoneOnce = False
                For l = 1 To .NumVotingDetailInitial
                  With .VotingDetailInitial(l)
                    .IdClient = RecClient!IdClient

                    If Not FlagDoneOnce Then
                      RecVotingDetail.AddNew

                      RecVotingDetail!IdClient = .IdClient
                      RecVotingDetail!RecordId = .RecordId
                      RecVotingDetail!NotUsed1 = .NotUsed1
                      RecVotingDetail!ClientNumber = .ClientNumber
                      RecVotingDetail!NotUsed2 = .NotUsed2
                      RecVotingDetail!SharesHeld = .SharesHeld
                      RecVotingDetail!DecimalIndicator = .DecimalIndicator
                      RecVotingDetail!NotUsed3 = .NotUsed3

                      .IdVotingDetail = RecVotingDetail!IdVotingDetail

                      FlagDoneOnce = True
                    End If

                    Field = FIELD_VOTING_DETAIL_FOR_VOTES & .ProposalRecordNumber
                    RecVotingDetail(Field) = .ForVotes
                    Field = FIELD_VOTING_DETAIL_AGAINST_VOTES & .ProposalRecordNumber
                    RecVotingDetail(Field) = .AgainstVotes
                    Field = FIELD_VOTING_DETAIL_ABSTAIN_VOTES & .ProposalRecordNumber
                    RecVotingDetail(Field) = .AbstainVotes
                  End With
                Next l

                If FlagDoneOnce Then
                  RecVotingDetail.Update
                  RecVotingDetail.MoveLast
                End If

                FlagDoneOnce = False
                For l = 1 To .NumVotingDetailRevised
                  With .VotingDetailRevised(l)
                    .IdClient = RecClient!IdClient

                    If Not FlagDoneOnce Then
                      RecVotingDetail.AddNew

                      RecVotingDetail!IdClient = .IdClient
                      RecVotingDetail!RecordId = .RecordId
                      RecVotingDetail!NotUsed1 = .NotUsed1
                      RecVotingDetail!ClientNumber = .ClientNumber
                      RecVotingDetail!NotUsed2 = .NotUsed2
                      RecVotingDetail!SharesHeld = .SharesHeld
                      RecVotingDetail!DecimalIndicator = .DecimalIndicator
                      RecVotingDetail!NotUsed3 = .NotUsed3

                      .IdVotingDetail = RecVotingDetail!IdVotingDetail

                      FlagDoneOnce = True
                    End If

                    Field = FIELD_VOTING_DETAIL_FOR_VOTES & .ProposalRecordNumber
                    RecVotingDetail(Field) = .ForVotes
                    Field = FIELD_VOTING_DETAIL_AGAINST_VOTES & .ProposalRecordNumber
                    RecVotingDetail(Field) = .AgainstVotes
                    Field = FIELD_VOTING_DETAIL_ABSTAIN_VOTES & .ProposalRecordNumber
                    RecVotingDetail(Field) = .AbstainVotes
                  End With
                Next l

                If FlagDoneOnce Then
                  RecVotingDetail.Update
                  RecVotingDetail.MoveLast
                End If

                FlagDoneOnce = False
                For l = 1 To .NumDirectorExceptionTotal
                  With .DirectorExceptionTotal(l)
                    .IdClient = RecClient!IdClient

                    If Not FlagDoneOnce Then
                      RecDirectorExceptionTotal.AddNew

                      RecDirectorExceptionTotal!IdClient = .IdClient
                      RecDirectorExceptionTotal!RecordId = .RecordId
                      RecDirectorExceptionTotal!NotUsed1 = .NotUsed1
                      RecDirectorExceptionTotal!ItemCount = .ItemCount
                      RecDirectorExceptionTotal!NotUsed2 = .NotUsed2

                      .IdDirectorExceptionTotal = RecDirectorExceptionTotal!IdDirectorExceptionTotal

                      FlagDoneOnce = True
                    End If

                    Field = FIELD_DIRECTOR_EXCEPTION_TOTAL_NAME & .Number
                    RecDirectorExceptionTotal(Field) = .Name
                    Field = FIELD_DIRECTOR_EXCEPTION_TOTAL_SHARES_WITHHELD & .Number
                    RecDirectorExceptionTotal(Field) = .SharesWithheld
                  End With
                Next l

                If FlagDoneOnce Then
                  RecDirectorExceptionTotal.Update
                  RecDirectorExceptionTotal.MoveLast
                End If

'                For l = 1 To .NumVotingDetailInitial
'                  With .VotingDetailInitial(l)
'                    .IdClient = RecClient!IdClient
'
'                    RecVotingDetailInitial.AddNew
'
'                    RecVotingDetailInitial!IdClient = .IdClient
'                    RecVotingDetailInitial!RecordId = .RecordId
'                    RecVotingDetailInitial!ProposalRecordNumber = .ProposalRecordNumber
'                    RecVotingDetailInitial!NotUsed1 = .NotUsed1
'                    RecVotingDetailInitial!ClientNumber = .ClientNumber
'                    RecVotingDetailInitial!NotUsed2 = .NotUsed2
'                    RecVotingDetailInitial!SharesHeld = .SharesHeld
'                    RecVotingDetailInitial!DecimalIndicator = .DecimalIndicator
'                    RecVotingDetailInitial!ForVotes = .ForVotes
'                    RecVotingDetailInitial!AgainstVotes = .AgainstVotes
'                    RecVotingDetailInitial!AbstainVotes = .AbstainVotes
'                    RecVotingDetailInitial!NotUsed3 = .NotUsed3
'
'                    RecVotingDetailInitial.Update
'                    RecVotingDetailInitial.MoveLast
'
'                    .IdVotingDetail = RecVotingDetailInitial!IdVotingDetail
'                  End With
'                Next l
'
'                For l = 1 To .NumVotingDetailRevised
'                  With .VotingDetailRevised(l)
'                    .IdClient = RecClient!IdClient
'
'                    RecVotingDetailRevised.AddNew
'
'                    RecVotingDetailRevised!IdClient = .IdClient
'                    RecVotingDetailRevised!RecordId = .RecordId
'                    RecVotingDetailRevised!ProposalRecordNumber = .ProposalRecordNumber
'                    RecVotingDetailRevised!NotUsed1 = .NotUsed1
'                    RecVotingDetailRevised!ClientNumber = .ClientNumber
'                    RecVotingDetailRevised!NotUsed2 = .NotUsed2
'                    RecVotingDetailRevised!SharesHeld = .SharesHeld
'                    RecVotingDetailRevised!DecimalIndicator = .DecimalIndicator
'                    RecVotingDetailRevised!ForVotes = .ForVotes
'                    RecVotingDetailRevised!AgainstVotes = .AgainstVotes
'                    RecVotingDetailRevised!AbstainVotes = .AbstainVotes
'                    RecVotingDetailRevised!NotUsed3 = .NotUsed3
'
'                    RecVotingDetailRevised.Update
'                    RecVotingDetailRevised.MoveLast
'
'                    .IdVotingDetail = RecVotingDetailRevised!IdVotingDetail
'                  End With
'                Next l
'
'                For l = 1 To .NumDirectorExceptionTotal
'                  With .DirectorExceptionTotal(l)
'                    .IdClient = RecClient!IdClient
'
'                    RecDirectorExceptionTotal.AddNew
'
'                    RecDirectorExceptionTotal!IdClient = .IdClient
'                    RecDirectorExceptionTotal!RecordId = .RecordId
'                    RecDirectorExceptionTotal!Number = .Number
'                    RecDirectorExceptionTotal!NotUsed1 = .NotUsed1
'                    RecDirectorExceptionTotal!Name = .Name
'                    RecDirectorExceptionTotal!ItemCount = .ItemCount
'                    RecDirectorExceptionTotal!SharesWithheld = .SharesWithheld
'                    RecDirectorExceptionTotal!NotUsed2 = .NotUsed2
'
'                    RecDirectorExceptionTotal.Update
'                    RecDirectorExceptionTotal.MoveLast
'
'                    .IdDirectorExceptionTotal = RecDirectorExceptionTotal!IdDirectorExceptionTotal
'                  End With
'                Next l
              End With
            Next k
          End With
        Next j

        For j = 1 To .NumTotal
          With .Total(j)
            .IdSolicitor = RecSolicitor!IdSolicitor

            RecTotal.AddNew

            RecTotal!IdSolicitor = .IdSolicitor
            RecTotal!RecordId = .RecordId
            RecTotal!NotUsed1 = .NotUsed1
            RecTotal!RecordCount = .RecordCount
            RecTotal!NotUsed2 = .NotUsed2

            RecTotal.Update
            RecTotal.MoveLast

            .IdTotal = RecTotal!IdTotal
          End With
        Next j
      End With
    Next i
  End With

  RecSolicitor.Close
  RecCUSIP.Close
  RecClient.Close
  RecVotingDetail.Close
'  RecVotingDetailInitial.Close
'  RecVotingDetailRevised.Close
  RecDirectorExceptionTotal.Close
  RecTotal.Close

  Bulkload = True
End Function

Sub BulkloadCaller()
  Dim ReturnValue As Boolean

  ReturnValue = Bulkload("C:\s00457rv.dat")
'  ReturnValue = Bulkload("C:\S457_TESTFILE_30708.TXT")
  If ReturnValue Then
    Debug.Print "Bulkload succeeded."
  Else
    Debug.Print "Bulkload failed."
  End If
End Sub
