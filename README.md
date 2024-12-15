# Simple web-server using C

## Overview

Halo, semuanya! Dalam repo ini, kami membuat suatu web-server sederhana dalam bahasa C dengan menggunakan fork() untuk mengatasi multi-client. Tujuan kami membuat program ini adalah untuk memenuhi nilai perkuliahan Sistem Operasi(Praktik) yang diampu oleh Bapak Muhammad Rizqi Sholahuddin.
Adapun program ini dibuat oleh mahasiswa atas nama:
- Nieto Salim Maula 231524019
- Thafa Fadillah Ramdani 231524027

## Fitur

Adapun fitur yang dibuat oleh kami dalam web-server yang telah dibuat, diantaranya:
- Koneksi multi-client dengan menggunakan fork()
- Laman website yang didesain untuk mengerjakan kuis
- Log hasil pengerjaan user disimpan ke dalam suatu file yang nantinya akan diolah untuk diurutkan secara descending

## Cara menggunakan

1. Pertama-tama, clone [repo ini!](https://github.com/AllThaf/simple-web-server-using-C)

```sh
git clone https://github.com/AllThaf/simple-web-server-using-C.git
```

2. Buka repository yang sudah di-clone tadi. Lalu, jalankan file Makefile

```sh
make
```

3. Buka laman [localhost:8000](localhost:8000) di browser anda. Anda juga bisa mengubah default port menjadi port yang anda inginkan. Port dapat dibuah di file `parsing.h`
