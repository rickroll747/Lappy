# Laper
An Effective Email/Net Worm In VBScript.

# Lappy.vbs/Laper.vbs
A simple VBS worm that spreads via HTML files, other files, email contacts, network shares and creates a backup copy of itself.
High-Level Functionality
Infects HTML files in the current directory, adding a script tag that references the VBS worm.
Infects other files in the current directory by copying the worm with different file extensions.
Sends itself as an email attachment to email contacts in Microsoft Outlook.
Spreads through network shares by copying itself to shared folders.
Creates a backup copy of the worm (backup_Laper.vbs) in the user's home directory.
Adds a registry key to ensure the worm runs automatically during login.
Disclaimer
This VBS worm is provided for educational purposes only. Misuse of this worm could lead to legal consequences and may harm others. The author is not responsible for any misuse or damage caused by this worm.

# What It Does
Infect HTML files: The worm searches for HTML files in the same folder and infects them by adding a script tag that references the worm.
Infect other files: The worm copies itself with various file extensions, infecting other files when they are opened or executed.
Send emails: The worm uses Microsoft Outlook to send itself as an attachment to email contacts in the user's address book.
Spread through network shares: The worm copies itself to network shares, spreading to other computers on the same network.
Autostart: The worm adds a registry key to ensure it runs automatically during login.
Backup copy: The worm creates a backup copy of itself in the user's home directory, ensuring persistence in case the original worm file is deleted.
Making It Extremely Difficult To Remove.

# Hacked By The Chinese!
