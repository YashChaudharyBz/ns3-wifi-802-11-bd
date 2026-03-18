# ns3 WiFi 802.11bd Implementation
Repo for implemention of WiFi 802.11bd in ns3.

To setup this code, download ns-3.38 first.
<br>
Or equivalently run:

```bash
wget -q https://www.nsnam.org/releases/ns-allinone-3.38.tar.bz2
tar xfj ns-allinone-3.38.tar.bz2
```

Now, to setup 802.11bd, clone this repo and run setup.sh after setting up NS3_PATH (path to the ns-3.38 folder).
<br>
Or equivalently run:

```bash
# use this path if you downloaded ns3 from above
export NS3_PATH="./ns-allinone-3.38/ns-3.38"
git clone https://github.com/YashChaudharyBz/ns3-wifi-802-11-bd.git
cd ns3-wifi-802-11-bd
./setup.sh
```

By running `setup.sh`, the code to use 802.11bd is placed in `ns-3.38/contrib`. Configure ns3 using `./ns3 configure` before use. The usage example is mentioned in the `ns-3.38/scratch/simulation` folder. It can be used like any other ns3 script, using `./ns3 run`.
