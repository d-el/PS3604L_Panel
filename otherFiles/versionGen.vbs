    '[Начало versionfile-gen.vbs]
    Dim Args
    Set Args = WScript.Arguments

    if (Args.Count < 2) then
        MsgBox "Error generate version file." + vbCrLf + "Usage:" + vbCrLf + "versionGen.vbs <filename>"
        WScript.Quit
    End If

    'Получаем имя выходного файла
    verfilename = Args(0)
    headerFileName = Args(1)

    'Получаем дату/время
    vYear  = CStr(Year  (Date))
    vMonth = CStr(Month (Date))
    if Len(vMonth) < 2 then
        vMonth = "0"+vMonth
    end if
    vDay    = CStr(Day    (Date))
    if Len(vDay) < 2 then
        vDay = "0"+vDay
    end if
    vHour  = CStr(Hour  (Time))
    if Len(vHour) < 2 then
        vHour = "0"+vHour
    end if
    vMinute = CStr(Minute (Time))
    if Len(vMinute) < 2 then
        vMinute = "0"+vMinute
    end if
    vSecond = CStr(Second (Time))
    if Len(vSecond) < 2 then
        vSecond = "0"+vSecond
    end if

    '
    Dim searchDate, searchMajor, searchMinor, searchRevision, searchBuild
    Dim MajorCurInt, MinorCurInt, RevisionCurInt, BuildCurInt
    Dim lineInFile
    Dim lineArray(256)
    Dim str
    searchDate      = "* @date     "
    searchMajor     = "* @major    "
    searchMinor     = "* @minor    "
    searchRevision  = "* @revision "
    searchBuild     = "* @build    "

    Set objFileToRead = CreateObject("Scripting.FileSystemObject").OpenTextFile(headerFileName, 1) 
    lineInFile = 0
    BuildCurInt = 0
    do while not objFileToRead.AtEndOfStream 
        lineArray(lineInFile) = objFileToRead.ReadLine()
        
        If InStr(lineArray(lineInFile), searchDate) > 0 Then
            str = Right(lineArray(lineInFile), Len(lineArray(lineInFile)) - Len(searchBuild))
            'Replace Date string
            lineArray(lineInFile) = Replace(lineArray(lineInFile), str, vDay + "." + vMonth + "." + vYear + " " + vHour + ":" + vMinute + ":" + vSecond)
        End If
        If InStr(lineArray(lineInFile), searchMajor) > 0 Then
            str = Right(lineArray(lineInFile), Len(lineArray(lineInFile)) - Len(searchMajor))
            MajorCurInt = CInt(str)
        End If
        If InStr(lineArray(lineInFile), searchMinor) > 0 Then
            str = Right(lineArray(lineInFile), Len(lineArray(lineInFile)) - Len(searchMinor))
            MinorCurInt = CInt(str)
        End If
        If InStr(lineArray(lineInFile), searchRevision) > 0 Then
            str = Right(lineArray(lineInFile), Len(lineArray(lineInFile)) - Len(searchRevision))
            RevisionCurInt = CInt(str)
        End If
        If InStr(lineArray(lineInFile), searchBuild) > 0 Then
            buildStrNum = lineInFile
            str = Right(lineArray(lineInFile), Len(lineArray(lineInFile)) - Len(searchBuild))
            BuildCurInt = CInt(str) + 1
            'Replace Build string
            lineArray(lineInFile) = Replace(lineArray(lineInFile), str, CStr(BuildCurInt))
        End If
        lineInFile = lineInFile + 1
    loop
    objFileToRead.Close 
    Set objFileToRead = Nothing

    if (BuildCurInt = 0) then
        MsgBox "Error in file: " + headerFileName
        WScript.Quit
    end if

    'Write header
    Set objFileToWrite = CreateObject("Scripting.FileSystemObject").OpenTextFile(headerFileName, 2, true, 0) 
    For i = 0 To lineInFile
        if (i < lineInFile) then
            objFileToWrite.Write lineArray(i)  + vbCrLf
        else
            objFileToWrite.Write lineArray(i)
        end if
    Next
    objFileToWrite.Close 
    Set objFileToWrite = Nothing

    'Создает исходник
    Set FileOutStream =  CreateObject("Scripting.FileSystemObject").OpenTextFile(verfilename, 2, true, 0)
    For i = 0 To buildStrNum + 1
        FileOutStream.Write lineArray(i)  + vbCrLf
    Next
    FileOutStream.Write vbCrLf

    FileOutStream.Write "#include ""version.h""" + vbCrLf + vbCrLf
    FileOutStream.Write "const uint32_t fwinfoMajor    = " + CStr(MajorCurInt) + ";" + vbCrLf
    FileOutStream.Write "const uint32_t fwinfoMinor    = " + CStr(MinorCurInt) + ";" + vbCrLf
    FileOutStream.Write "const uint32_t fwinfoRevision = " + CStr(RevisionCurInt) + ";" + vbCrLf
    FileOutStream.Write "const uint32_t fwinfoBuild    = " + CStr(BuildCurInt) + ";" + vbCrLf
    FileOutStream.Write "const char fwinfoVersion[] = {""" + CStr(MajorCurInt) + "." + CStr(MinorCurInt) + "." + CStr(RevisionCurInt) + "." + CStr(BuildCurInt) + """};" + vbCrLf
    FileOutStream.Write "const char fwinfoDate[]    = {""" + vDay + "." + vMonth + "." + vYear + """};" + vbCrLf
    FileOutStream.Write "const char fwinfoTime[]    = {""" + vHour + ":" + vMinute + ":" + vSecond + """};" + vbCrLf

    For i = buildStrNum + 2 To lineInFile
        if (i < lineInFile) then
            FileOutStream.Write lineArray(i)  + vbCrLf
        else
            FileOutStream.Write lineArray(i)
        end if
    Next
        
    '[Конец versionfile-gen.vbs]