$path = "C:\Users\phisn\source\repos\PixelJumper2\Client"

$year = $NumberOfDocs = $NumberOfWords = $null

$i = 1

$totalDocs = (Get-ChildItem E:\Data\ScriptingGuys -filter "*doc*" -Recurse -file |

  Where {$_.BaseName -match '^(HSG|WES|QHF)'}).count

$word = New-Object -ComObject word.application

$word.visible = $false

Get-ChildItem $path -filter "????" -Directory |

 ForEach-Object {

   $year = $_.name

   Get-ChildItem $_.FullName -filter "*doc*" -Recurse -file |

     Where-Object {$_.BaseName -match '^(HSG|WES|QHF)'} |

      ForEach-Object {

      $i++

      Write-Progress -Activity "Processing $($_.BaseName)" `

       -PercentComplete  (($i / $totalDocs)*100) -Status "Working on $year"

      $document = $word.documents.open($_.fullname)

      $NumberOfWords += $document.words.count

      $NumberOfDocs ++

      $document.close() | out-null

      [System.Runtime.Interopservices.Marshal]::ReleaseComObject($document) |

       Out-Null

      Remove-Variable Document }

    [PSCustomObject]@{

     "NumberOfDocuments" = $NumberOfDocs

     "NumberOfWords" = $NumberOfWords

     "Year" = $year}

     $NumberOfDocs = $NumberOfWords = $year = $null }

$word.quit()

[System.Runtime.Interopservices.Marshal]::ReleaseComObject($word) | Out-Null

Remove-Variable Word

[gc]::collect()

[gc]::WaitForPendingFinalizers()