# 捕蚊燈專案建置流程

Alan 提供的教學
https://hackpad.com/IoT--r3w2zmI978r

## 介紹
這是用來介紹如何從 0 土炮改裝捕蚊燈，變成可以和 Linkit smart 7688 duo 溝通的流程

## 材料
1. [捕蚊燈](http://24h.pchome.com.tw/prod/DMAL4M-A9005ZDTR)
2. MTK Linkit Smart 7688 "Duo" (NOT 7688)
3. 洞洞板 *2
用來裝上 Linkit Smart 7688 duo 以及焊接測量電壓的電路還有降壓模組
4. 七段顯示器 "模組"
5. 1M 歐姆電阻 + 2k 歐姆電阻
6. 3.6 V 齊納二極體 (Zener二極體)
7. 110V 轉 5V 降壓模組

## Demo code
https://github.com/nalanala/iot-electric-mosquito-killer/blob/master/iot_electronic_mos_killer.ino

## 配線
這個專案要焊接的主要是兩個部分
1. 兩個電阻搭配 Zener 組成的電路
2. 110V to 5V (AC-DC) 降壓模組的焊接

### 第一部份的電路
![](https://i.imgur.com/HKKcvb7.png)
Vin: 捕蚊燈電柵的正位電壓，拆開捕蚊燈後，會看到兩條黃色的線，這是左邊的那一條（左+右-)

GND: 是指電柵的接地電壓，也是黃色的，會是右邊的那一條。實際在焊接的時候，兩個 GND 會接在一起，並接到開發板的 GND 上。（左邊降壓電路的 GND 還會有一條接到電柵上，但是就要用多芯線）

V_SENSE: 用來接 7688 duo 的 Digital port (Interrupt port) 來感測電壓的變化值

此外要注意的是板子的 GND 也要接到電柵的 GND 讓他們共地，代表他們是同一個基於基準電壓。

Vin 因為經由捕蚊燈 110V 的交流電升壓整流之後，變成了 1000 V, 所以如果你的手去碰到這個 Vin 端的電路，就會變成導通，人會往後彈，要注意一下。


電柵跟這個電路的示意圖如下：
![](https://i.imgur.com/F4Yi40Y.jpg)


### 第二部分電路 -- 供電電路
這是用來讓捕蚊燈的電源可以供給給 Linkit Smart 7688 duo 的，原本的捕蚊燈只能夠使用 USB 供電。

使用的是 110V to 5V 模組 (AC-to-DC)，實際上的名稱叫做：AC-DC 隔離電源模組

模組的樣子大概長這樣：
![](http://www.buyic.com.tw/images/product_images/popup_images/04011000011.jpg)


這個模組有 4個地方，分別要怎麼接？

兩個 AC 腳位怎麼接：
- 一個循著捕蚊燈電源線接到模組 AC
- 一個循著捕蚊燈 AC 的接地，接到模組 AC

Vo 怎麼接：
- -Vo: 接到板子上的 GND
- +Vo: 如果要使用的話，接到板子上的 Vcc

#### [特別注意，免得電腦變磚塊]
當我們要讓板子自行運作的時候，就將 +Vo 接到開發板 Vcc。燒錄開發板的時候，記得要將它拔起來，以免干擾訊號，導致無法燒錄。更甚者，因為通常轉出來的電壓也不見得是 5V, 如果你電腦的工作電壓比它轉出來的小的很，資料寫入的方向就不一樣，你的電腦就會變成磚了！


## 注意事項
![](http://www.cnx-software.com/wp-content/uploads/2015/12/Link_Smart_7688_DUO_Pinout.png)
Linkit Smart 7688 duo DataSheet 圖片是錯誤的，如果需要參考腳位可以參考:
https://www.arduino.cc/en/Main/ArduinoBoardLeonardo

對於 Digital pin 來說：
pinMode 的設定是對應到 Digital Pin, 在 DataSheet 以 DX 顯示。例如： pinMode(7, INPUT) 就是設定 D7 為 input port.

```
attachInterrupt(4, pinChanged, CHANGE);
這裡的 4 並不是 D4 port 而是對應到 Interrupt 的 port
```

Arduino Leonardo 的文件有寫到這一段
> External Interrupts: 3 (interrupt 0), 2 (interrupt 1), 0 (interrupt 2), 1 (interrupt 3) and 7 (interrupt 4)

也就是說，如果你要設定 Digital Pin 2 的 interrupt, 要 
```
pinMode(2, INPUT);  attachInterrupt(1, pinChanged, CHANGE);
```

## FAQ

由張嘉幃同學提供解答
https://hackmd.io/MYIwzGCGAcAMwFoCcAzAbEhAWYB2NyYArLggEzS5ZkqQpKwAmWQA

1. 捕蚊燈的電路為什麼需要 整流 跟 倍壓電路？
>因為AC 110V很難在瞬間電死蚊子，所以需要把電壓放大，瞬間的電流值也會跟著變大(R為定值，V變大I也會跟著變大)，蚊子就會瞬間GG了[name=Zhang Jiawei]


2. 齊納二極體加上兩個電阻所構成的電路主要用途是為了降壓讓開發版得以測量電壓嗎？

對整個工作流程來說，我的理解是電柵一般時候等同斷路，所以我們利用電阻，齊納二極體做了並聯電路之後，~~電柵電流就可以通過並連電流~~ **Zener的用途是防止突波，平時狀況下不太作動** 我們 Vsense 量測到的便會是 2K 歐姆的電壓差。如果電到蚊子的時候，~~並聯電路~~ **整個電路**不會有電流通過（電柵短路）所以量測到的 Vsense = 0? 所以藉由量測到的電壓變化，來測量是否有電到蚊子
>是ㄉ[name=Zhang Jiawei]

3. 為什麼齊納二極體所構成的電路使用的電阻是 1M, 2K 歐姆的電阻？（是不是跟量測電壓數值大小有關？Hackpad 上面寫 2V 就會被判斷為低電壓

還是因為這是為了計算不要讓 Pin 腳的電壓高過工作電壓？
>True，同時大的電阻也可以使我們拉到控制板的分壓電路被分到的總電流變少，使我們整個電路的總電壓差能夠維持在高壓[name=Zhang Jiawei]

4. 可不可以再請教一次齊納二極體的功用是什麼？

今天有說過齊納二極體在這裡主要是防突波，但是為什麼可以防突波？瞬間大電壓

>因為在開關瞬間通路的時候，有可能發生(尤其在傳統的硬式開關)電荷快速大量流入(電流的定義:每單位時間所流入的電荷數)，瞬間造成的大電壓有機率會造成電路的損毀，Zener在這邊就是為了防止端電壓大於我們的工作電壓。[name=Zhang Jiawei]
>嘛就是個以防萬一的概念吧XD [name=Zhang Jiawei]

5. 測量電壓的時候，主要是測量電壓變成 0 的時候，但是電壓為什麼會變成 0? 齊納二極體那條電路會短路嗎？（原本圖上是畫電柵會變成短路

>跟Zener沒啥關係喔！
>筆記上面有劃記到，實際上可以分成這樣：
>[點我點我]("http://falstad.com/circuit/circuitjs.html?cct=$+1+0.000005+10.20027730826997+50+5+43%0Av+96+384+96+112+0+0+40+5+0+0+0.5%0Aw+96+112+272+112+0%0Aw+272+112+272+176+0%0Aw+272+176+208+176+0%0Aw+208+176+208+240+0%0Aw+272+176+368+176+0%0Ab+207+131+118+86+0%0Ax+128+116+196+119+0+24+Circuit%0Aw+96+384+272+384+0%0Ab+204+344+121+405+0%0Ax+127+381+195+384+0+24+Circuit%0Aw+208+272+208+320+0%0Aw+208+320+272+320+0%0Aw+272+320+272+384+0%0Aw+272+320+368+320+0%0Ar+368+176+368+240+0+1000000%0Ar+368+240+368+320+0+2000%0Az+448+320+448+240+1+0.805904783+5.6%0Aw+368+240+448+240+0%0Aw+368+320+448+320+0%0Aw+448+240+496+240+0%0A210+496+240+560+240+0+10240%0A")
>中間斷開的地方是電死蚊子的地方，你可以看到，如果蚊子上去的話，右邊的電路兩端的電壓會瞬間等壓差 = 0V　造成近似瞬間短路的情形[name=Zhang Jiawei]

6.  今天有查到 pin 2 port 是代號 1 的 interrupt 腳位，這在哪裡可以查到呢？

>看晶片號查Datasheet，剛好7688 duo的微控制器是跟Arduino leonardo是同一塊，所以可以兼用[name=Zhang Jiawei]
>https://www.arduino.cc/en/Main/ArduinoBoardLeonardo
>寫在下面喔~

7. 如果接了 110V - 5V 降壓模組之後，將捕蚊燈的電供電給板子後，我們如果接上 USB 燒 code, 查看 Serial 會有影響嗎？
>原則上共電源這件事本身就有點危險就是了，電壓較高的那一邊會朝較低的那一側流電流(有壓差就有電流)[name=Zhang Jiawei]
>雖然在理想狀況下，兩邊等5V的時候沒什麼影響，但不管是USB Port或者是降壓模組都不是理想5V，在有許多的變項的狀況下，務必多加注意就是了[name=Zhang Jiawei]


>或是準備一個沒有人要用的電腦(ry[name=Zhang Jiawei]
