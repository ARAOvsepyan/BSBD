# Цель работы
Ознакомление и установка дистрибутива Linux на виртуальную машину.
# Ход работы
## Настройка VirtualBox
Ознакомление и установка дистрибутива Linux на виртуальную машину.
![Oracle VM](https://raw.githubusercontent.com/ARAOvsepyan/BSBD/main/LR1/img/VB.PNG)
## Работа с дисками
После начала установки необходимо произвести разбиение диска на разделы, для этого используется утилита `fdisk`.
```
/dev/sda1 - 512MB	EFI 	EFI system
/dev/sda2 - 2GB		swap 	Linux swap
/dev/sda3 - 10GB	home 	Linux filesystem
/dev/sda4 - 10GB	var 	Linux filesystem
/dev/sda5 - 10GB	opt  	Linux filesystem
/dev/sda6 - 20.5GB	root 	Linux filesystem
```
Форматирование разделов с помощью утилиты `mkfs`.
```shell
mkfs.fat -F32 /dev/sda1
mkswap /dev/sda2
mkfs.ext4 /dev/sda3
mkfs.ext4 /dev/sda4
mkfs.ext4 /dev/sda5
mkfs.ext4 /dev/sda6
```
Монтирование разделов:
```shell
mount /dev/sda6 /mnt
mkdir -p -v /mnt/boot/efi
mount /dev/sda1 /mnt/boot/efi
mkdir -p -v /mnt/{home,var,opt}
mount /dev/sda3 /mnt/home
mount /dev/sda4 /mnt/var
mount /dev/sda5 /mnt/opt
swapon /dev/sda2
```
Результат:

![partisions](https://raw.githubusercontent.com/ARAOvsepyan/BSBD/main/LR1/img/partisions.PNG)

## Установка ОС
Установка операционной системы производится с помощью утилиты `pacstrap`.
```shell
pacstrap /mnt base linux linux-firmware
genfstab -U /mnt >> /mnt/etc/fstab
arch-chroot /mnt
```
Настройка времени
```shell
ln -sf /usr/share/zoneinfo/Europe/Moscow /etc/localtime
hwclock --systohc
```

Произведена установка текстового редактора **nano** для дальнейшего редактирования конфигурационных файлов.
```shell
pacman -S nano
```

*/etc/locale.gen* 
раскоментированы строки:

> en_US.UTF-8 UTF-8
>
> ru_RU.UTF-8 UTF-8

*/etc/hostname*
добавлена строка:

> ara

*/etc/hosts*
добавлена строка:

> 127.0.0.1 localhost
>
> ::1 localhost
>
> 127.0.1.1 ara.localdomain ara

Настройка локали, создание начального загрузочного диска системы и установка пароля для **root**:

```shell
locale-gen
mkinitcpio -P
passwd
```

## Установка загрузчика
Для Arch Linux согласно Wiki самым лучшим загрузчиком является GRUB.
```shell
mkdir -p /boot/efi/EFI/arch
cp /boot/vmlinuz-linux /boot/efi/EFI/arch/
cp /boot/initramfs-linux.img /boot/efi/EFI/arch/
cp /boot/initramfs-linux-fallback.img /boot/efi/EFI/arch/

pacman -S grub grub-btrfs efibootmgr
grub-install --target=x86_64-efi --efi-directory=/boot/efi --bootloader-id=GRUB
grub-mkconfig -o /boot/grub/grub.cfg
```
Теперь необходимо установить утилиты для работы ОС, затем отмонтировать все разделы и перезагрузить систему.
```shell
pacman -S networkmanager openssh net-tools sudo

exit
umount -R /mnt
reboot
```
## Вторичная настройка системы
### Сеть
Перезапуск сетевых служб:
```shell
systemctl restart NetworkManager
systemctl enable NetworkManager
systemctl restart systemd-networkd
```
### Настройки пользователя
1. Создание пользователя **ovsepyan** с установкой пароля
2. Создание группы **ic8-63**
3. Добавления пользователя **ovsepyan** в группу **ic8-63** и в группу **wheel** *необходимо для выдачи прав администратора*
```shell
groupadd ic8-63
useradd -g ic8-63 -m ovsepyan
passwd ovsepyan
usermod -aG wheel ovsepyan
echo "ovsepyan ALL=(ALL) ALL" >> /etc/sudoers
```
## Настройка ssh
Запрет аутентификации по паролю, разрешение доступа пользователю **ovsepyan**
Содержание файлов конфигурации после настройки:

*/etc/ssh/ssh_config*

> ...
>
> PasswordAuthentication no

*/etc/ssh/sshd_config*

> ...
>
> AllowUsers ovsepyan

Запуск службы *SSH*

```shell
systemctl start sshd
systemctl enable sshd
```
Результат настройки **SSH**.

![](https://raw.githubusercontent.com/ARAOvsepyan/BSBD/main/LR1/img/ssh_check.PNG)

![](https://raw.githubusercontent.com/ARAOvsepyan/BSBD/main/LR1/img/ssh_check_1.png)

## Установка XORG и KDE Plasma
Установка XORG и KDE Plasma как рабочего окружения.

```shell 
pacman -S xf86-video-vesa xorg-server xorg-xinit xorg-drivers

pacman -S plasma kde-applications sddm
pacman -S plasma kde-applications sddm
systemctl enable sddm
reboot
```
Результат установки KDE Plasma

![](https://raw.githubusercontent.com/ARAOvsepyan/BSBD/main/LR1/img/kde.PNG)

![](https://raw.githubusercontent.com/ARAOvsepyan/BSBD/main/LR1/img/KDE_Desktop.PNG)

Также необходимо поставить браузер и произвести установку Clion.

```shell 
pacman -S opera fuse
```

Скачивание JetBrains Toolbox производится с официального сайта *[jetbrains](https://www.jetbrains.com/ru-ru/toolbox-app/)*

```shell
tar -xf Downloads/jetbrains-toolbox-1.20.7940.tar.gz
rm Downloads/jetbrains-toolbox-1.20.7940.tar.gz
cd jetbrains-toolbox-1.20.7940
./jetbrains-toolbox
```
![](https://raw.githubusercontent.com/ARAOvsepyan/BSBD/main/LR1/img/clion.PNG)

# Вывод

В результате выполнения данной лабораторной работы был установлен дистрибутив Arch Linux на виртуальную машину.

## Дополнительные ресурсы

[Руководство по установке Arch Linux](https://wiki.archlinux.org/index.php/installation_guide)

[Руководство по установке GRUB](https://wiki.archlinux.org/index.php/GRUB)

[Настройка SSH](https://wiki.archlinux.org/index.php/OpenSSH)

[Настройка Locale](https://wiki.archlinux.org/index.php/Locale)

