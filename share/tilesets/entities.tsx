<?xml version="1.0" encoding="UTF-8"?>
<tileset version="1.10" tiledversion="1.12.2" name="entities" tilewidth="56" tileheight="162" tilecount="7" columns="0">
 <grid orientation="orthogonal" width="1" height="1"/>
 <tile id="0" type="spawn">
  <image source="entities/spawn.png" width="56" height="80"/>
 </tile>
 <tile id="1" type="coin">
  <image source="entities/coin.png" width="54" height="54"/>
 </tile>
 <tile id="2" type="goal">
  <image source="entities/goal.png" width="54" height="162"/>
 </tile>
 <tile id="3" type="enemy">
  <properties>
   <property name="sprite" value="gnu"/>
   <property name="speed" type="float" value="90"/>
  </properties>
  <image source="entities/enemy_gnu.png" width="56" height="80"/>
 </tile>
 <tile id="4" type="enemy">
  <properties>
   <property name="sprite" value="tux"/>
   <property name="speed" type="float" value="110"/>
  </properties>
  <image source="entities/enemy_tux.png" width="56" height="80"/>
 </tile>
 <tile id="5" type="enemy">
  <properties>
   <property name="sprite" value="wilber"/>
   <property name="speed" type="float" value="70"/>
  </properties>
  <image source="entities/enemy_wilber.png" width="56" height="80"/>
 </tile>
 <tile id="6" type="enemy">
  <properties>
   <property name="sprite" value="kisi"/>
   <property name="speed" type="float" value="130"/>
  </properties>
  <image source="entities/enemy_kisi.png" width="56" height="80"/>
 </tile>
</tileset>
