param($obj, $out)
$b = [IO.File]::ReadAllBytes($obj)

# Читаем смещение PE-заголовка из DOS-заголовка (адрес 0x3C)
$pe_offset = [BitConverter]::ToUInt32($b, 0x3C)

# Читаем данные из COFF-заголовка внутри PE (смещения фиксированы относительно $pe_offset)
$cnt = [BitConverter]::ToUInt16($b, $pe_offset + 6)   # Количество секций
$opt = [BitConverter]::ToUInt16($b, $pe_offset + 20)  # Размер Optional Header
$sec = $pe_offset + 24 + $opt                         # Начало таблицы секций

for ($i = 0; $i -lt $cnt; $i++) {
    $o    = $sec + 40 * $i
    $name = [Text.Encoding]::ASCII.GetString($b, $o, 8).TrimEnd([char]0)
    
    # Линкер в режиме /DRIVER может назвать секцию .text, INIT, PAGE или .text$mn. 
    # Проверяем на наличие слова text или просто берем первую секцию
    if ($name -like '*text*' -or $name -eq 'INIT' -or $name -eq 'PAGE') {
        
        $size = [BitConverter]::ToUInt32($b, $o + 16) # VirtualSize (размер кода в памяти)
        $raw  = [BitConverter]::ToUInt32($b, $o + 20) # PointerToRawData (смещение в файле)
        $nrel = [BitConverter]::ToUInt16($b, $o + 32)
        
        # Если линкер округлил физический размер на диске, берем VirtualSize, 
        # чтобы в бинарник не попали лишние нули выравнивания
        $code = New-Object byte[] $size
        [Array]::Copy($b, $raw, $code, 0, $size)
        [IO.File]::WriteAllBytes($out, $code)
        
        Write-Host "OK: $size bytes extracted from section [$name]"
        if ($nrel -gt 0) { Write-Host "WARNING: $nrel relocations, code is not position independent" }
        exit 0
    }
}
Write-Host "ERROR: Section not found"; exit 1