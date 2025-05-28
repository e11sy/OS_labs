Write-Output "Making 1 writer for init of buffer.txt..."
Start-Process .\main.exe -ArgumentList "writer"
Start-Sleep -Seconds 3

Write-Output "Making 9 readers..."
1..10 | ForEach-Object {
    Write-Output "Starting reader $_"
    Start-Process .\main.exe -ArgumentList "reader"
    Start-Sleep -Milliseconds 200
}

Write-Output "Making 9 writers..."
1..9 | ForEach-Object {
    Write-Output "Starting writer $_"
    Start-Process .\main.exe -ArgumentList "writer"
    Start-Sleep -Milliseconds 200
}

Write-Output "All processes are running!"
