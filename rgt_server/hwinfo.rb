#!/usr/bin/env ruby
require 'rbconfig'
#require 'pp'

def GetHost
    RbConfig::CONFIG["host_os"]
end

host = GetHost()

if host =~ /mingw/
    require 'win32/registry'
    require 'Win32API'
    def GetCPUInfo
        cpu_info = {}
        Win32::Registry::HKEY_LOCAL_MACHINE.open(
                'HARDWARE\\DESCRIPTION\\System\\CentralProcessor') do |reg|
            reg.each_key do |k, v|
                reg.open(k) do |cpu|
                    cpu_info[k.to_i] = cpu["~MHz"].to_f
                end
            end
        end
        cpu_info
    rescue ArgumentError
    end
    GlobalMemoryStatusEx_ = Win32API.new('kernel32', 'GlobalMemoryStatusEx', 'P', 'V')
    MEMORYSTATUSEX = Struct.new(:length, :memoryLoad, :totalPhys, :availPhys,
        :totalPageFile, :availPageFile, :totalVirtual, :availVirtual, :availExtendedVirtual)
    MEMORYSTATUSEX.class_eval do
        def pack
            length = 7 * 8 + 4 * 2
            [length, memoryLoad, totalPhys, availPhys, totalPageFile,
                availPageFile, totalVirtual, availVirtual, availExtendedVirtual
                ].pack('l2Q7')
        end
        def self.unpack(s)
            new(*s.unpack('l2Q7'))
        end
    end
    def GlobalMemoryStatusEx
        buf = MEMORYSTATUSEX.new(0,0,0,0,0,0,0,0,0)
        buf_s = buf.pack
        GlobalMemoryStatusEx_.call(buf_s)
        MEMORYSTATUSEX.unpack(buf_s)
    end
    GetLogicalDriveStringsA_ = Win32API.new('kernel32', 'GetLogicalDriveStringsA', ['L','P'], 'L')
    def GetLogicalDriveStringsA
        bufl = 512
        buf = " " * bufl
        GetLogicalDriveStringsA_.call(bufl, buf)
        buf = buf.strip.split("\x00")
        return buf
    end
    GetDriveType_ = Win32API.new('kernel32', 'GetDriveType', ['P'], 'L')
    def GetDriveType(drive_letter)
        GetDriveType_.call(drive_letter)
    end
    GetDiskFreeSpace_ = Win32API.new('kernel32', 'GetDiskFreeSpace', ['P', 'P', 'P', 'P', 'P'], 'B')
    def GetDiskFreeSpace(drive_letter)
        sectorsPerCluster = " " * 4
        bytesPerSector = " " * 4
        numberOfFreeClusters = " " * 4
        totalNumberOfClusters = " " * 4
        GetDiskFreeSpace_.call(drive_letter, sectorsPerCluster, bytesPerSector,
                               numberOfFreeClusters, totalNumberOfClusters)
        sectorsPerCluster = sectorsPerCluster.unpack('L')[0]
        bytesPerSector = bytesPerSector.unpack('L')[0]
        numberOfFreeClusters = numberOfFreeClusters.unpack('L')[0]
        totalNumberOfClusters = totalNumberOfClusters.unpack('L')[0]
        bytesPerCluster = sectorsPerCluster * bytesPerSector
        return [numberOfFreeClusters * bytesPerCluster, totalNumberOfClusters * bytesPerCluster]
    end

    def BtoMB(b)
        b / (1024 * 1024)
    end

    DRIVE_FIXED = 3

    def GetDisks
        GetLogicalDriveStringsA().find_all {|dl| GetDriveType(dl) == DRIVE_FIXED}
    end

    def GetDiskInfo(disk)
        free, total = GetDiskFreeSpace(disk).map {|x| BtoMB(x)}
        {total: total, free: free}
    end

    def GetMemInfo
        memstat = GlobalMemoryStatusEx()
        {total: BtoMB(memstat.totalPhys), free:BtoMB(memstat.availPhys)}
    end

    def GetDisksInfo
        disks = GetDisks()
        disks_info = {}
        disks.each do |disk|
            disks_info[disk] = GetDiskInfo(disk)
        end
        disks_info
    end
    def SetLowestPriority
        proc_hndl = Win32API.new('kernel32','GetCurrentProcess','V','L').call()
        Win32API.new('kernel32', 'SetPriorityClass', ['L', 'L'], 'B').call(proc_hndl, 0x40)
    end
elsif host =~ /linux/
    def GetDisksInfo
        filesystems = (`df -hlP -B 1M`).split("\n").map {|l| l.split(' ') }
        filesystems.slice!(0, 1)
        filesystems = filesystems.find_all {|l| l[0] != 'none'}
        disks_info = {}
        filesystems.each do |fs|
            disk = fs[-1]
            total = fs[1].to_i
            free = fs[3].to_i
            disks_info[disk] = {total: total, free: free}
        end
        disks_info
    end

    def GetMemInfo
        memf = File.new('/proc/meminfo', 'r')
        total = memf.gets.split(' ')[1].to_i / 1024
        free = memf.gets.split(' ')[1].to_i / 1024
        {total: total, free: free}
    end

    def GetCPUInfo
        cpu_dirs = Dir.glob('/sys/devices/system/cpu/cpu[0-9]*')
        freq_suf = '/cpufreq/cpuinfo_max_freq'
        cpu_info = {}
        cpu_dirs.each do |cpu_dir|
            cpu_dir =~ /\/cpu(\d*)$/
            cpu_n = $1.to_i
            cpu_info[cpu_n] = File.new(cpu_dir + freq_suf).gets.chomp.to_f / 1000
        end
        cpu_info
    end

    def SetLowestPriority
        Process.setpriority(Process::PRIO_PROCESS, 0, 19)
    end
end

def GetHWInfo
    {cpu: GetCPUInfo(), disk: GetDisksInfo(), memory: GetMemInfo()}
end

#PP.pp GetHWInfo()

