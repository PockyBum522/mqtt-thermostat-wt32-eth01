$hostAddress = "192.168.1.43"

cd ..

# Build it
pio run

Write-Host ""

# Now upload it
$realCurlPath = "C:/PortableApplications/linuxCurl/bin/curl.exe";

$newFirmwarePath = ".pio/build/wt32-eth01/firmware.bin"

$md5 = (Get-FileHash $($newFirmwarePath) -Algorithm MD5).Hash

$mode = "firmware"

Write-Host ""
Write-Host "Testing connection to $($hostAddress):"
$connectionResult = Test-NetConnection -ComputerName $hostAddress -InformationLevel "Detailed"

# Verify the host is responding before we bother trying to send new fi
if ($connectionResult.PingSucceeded)
{
    Write-Host ""
    Write-Host "$hostAddress - Found!"

    Write-Host ""
    Write-Host "Posting: $($newFirmwarePath)"
    Write-Host ""

    # Send it!
    ."$($realCurlPath)" --digest --compressed -L -X POST -F "MD5=$($md5)" -F "name=$($mode)" -F "data=@$($newFirmwarePath);filename=$($mode)" "$($hostAddress)/update"
}
else
{
    Write-Host ""
    Write-Host "$hostAddress did not respond. Check connection and try again. Aborting..."
    Write-Host ""
}

# Go back in so we're in the right dir to run this script again next time
cd buildScripts