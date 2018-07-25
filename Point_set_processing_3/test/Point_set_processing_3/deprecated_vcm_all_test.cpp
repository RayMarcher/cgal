#include <CGAL/internal/disable_deprecation_warnings_and_errors.h>

#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/vcm_estimate_edges.h>
#include <CGAL/property_map.h>
#include <CGAL/IO/read_off_points.h>

#include <boost/dynamic_bitset.hpp>

#include <vector>
#include <fstream>


// Types
typedef CGAL::Exact_predicates_inexact_constructions_kernel Kernel;
typedef Kernel::Point_3 Point;
typedef Kernel::Vector_3 Vector;

typedef CGAL::cpp11::array<double,6> Covariance;


bool test_fandisk()
{
std::cout << "=== test_fandisk ===\n";
  std::size_t nb_points = 15843;
  CGAL::Identity_property_map<Point> pmap;
  Covariance expected;
  int index;
  
  // Reads a .xyz point set file in points[].
  std::vector<Point> points;
  std::vector<Covariance> cov;
  boost::dynamic_bitset<std::size_t> on_feature_edge(nb_points);
  points.reserve(nb_points);
  cov.reserve(nb_points);
  std::ifstream stream("data/fandisk.off");
  if (!stream ||
      !CGAL::read_off_points(stream,
                             std::back_inserter(points),
                             pmap))
  {
    std::cerr << "Error: cannot read file data/fandisk.off" << std::endl;
    return false;
  }

// test vcm without convolution
  std::cout << " Test compute vcm without convolution\n";
  cov.clear();
  CGAL::compute_vcm(points.begin(), points.end(), pmap, cov, 0.2, 0, Kernel());

//    check the cov matrices
  std::ifstream cov_stream("vcm_all_test-covmat_no_cov.txt");
  while( cov_stream >> index >> expected[0] >> expected[1] >> expected[2] >> expected[3] >> expected[4] >> expected[5] )
    for(int i=0;i<6;++i)
      assert( CGAL::abs(cov[index][i]-expected[i]) <= CGAL::abs(expected[i])*5/100 );
  cov_stream.close();

//    check points detected as feature point
  int indices_no_cov[243]={81,616,694,726,740,812,814,823,834,835,874,1153,1170,1188,1273,1382,1415,1563,1720,1745,1752,1763,1825,2013,2041,2042,2065,2181,2463,2508,2573,2616,2630,2632,2654,2715,2842,2864,2867,2893,3006,3014,3018,3121,3122,3144,3382,3654,3740,3914,4392,4454,4549,4765,4774,4804,4843,4862,4918,4949,4980,5054,5150,5178,5179,5181,5183,5373,5374,5557,5583,5584,5655,5657,5767,5827,6380,6565,6619,6645,6784,6786,6839,6895,6896,6929,7064,7071,7074,7098,7239,7350,7545,7659,7661,7681,7709,7843,7859,7895,7973,7983,8003,8064,8115,8125,8143,8255,8334,8428,8429,8464,8554,8689,8768,8769,8881,8883,8924,8962,8963,9121,9168,9213,9267,9380,9495,9503,9536,9577,9648,9661,9665,9668,9695,9696,9740,9753,9798,9805,9821,9972,9977,10095,10096,10254,10266,10267,10269,10297,10305,10671,10725,10727,10824,10915,10948,11015,11026,11201,11234,11455,11466,11553,11611,11667,11713,11724,11745,11849,11903,11952,11992,12008,12287,12297,12321,12342,12467,12468,12511,12530,12715,12758,12987,13012,13045,13188,13210,13380,13442,13443,13475,13503,13549,13564,13572,13591,13630,13672,13694,13740,13742,13791,13856,13896,13905,13942,13945,14008,14055,14077,14140,14166,14397,14460,14482,14485,14546,14566,14614,14667,14721,14793,14816,14864,14928,15029,15037,15090,15132,15148,15168,15360,15363,15384,15394,15478,15501,15522,15810,15811,15827};
  on_feature_edge.reset();
  for(int i=0;i<243;++i)
    on_feature_edge.set(indices_no_cov[i]);
  std::size_t nb_wrong=0;
  for (std::size_t i=0; i<points.size(); ++i)
    if( on_feature_edge[i] != CGAL::vcm_is_on_feature_edge(cov[i], 0.18)) ++nb_wrong;
  assert( nb_wrong < 20 ); // be tolerant to numeric errors

// test vcm with convolution
  std::cout << " Test compute vcm with convolution\n";
  cov.clear();
  CGAL::compute_vcm(points.begin(), points.end(), pmap, cov, 0.2, 0.1, Kernel());
//    check the cov matrices
  cov_stream.open("vcm_all_test-covmat_with_cov.txt");
  while( cov_stream >> index >> expected[0] >> expected[1] >> expected[2] >> expected[3] >> expected[4] >> expected[5] )
    for(int i=0;i<6;++i)
      assert( CGAL::abs(cov[index][i]-expected[i]) <= CGAL::abs(expected[i])*5/100 );
  cov_stream.close();
//    check points detected as feature point
  int indices_with_cov[2746]={21,22,23,24,25,36,37,38,42,43,44,45,77,78,79,80,81,82,83,89,120,121,129,130,173,174,181,182,184,185,186,215,216,221,222,228,232,233,234,236,255,256,269,270,271,275,288,289,290,291,292,293,294,315,317,329,354,355,356,378,379,380,381,382,383,402,403,404,405,419,420,467,469,477,478,479,480,482,483,497,498,499,521,527,528,536,557,581,582,586,587,593,594,613,614,616,617,618,619,624,625,627,629,644,678,693,694,695,696,700,725,726,727,734,735,736,740,741,742,755,791,794,798,811,812,813,814,823,824,825,826,829,830,831,835,844,850,851,858,859,860,861,874,875,876,927,928,936,945,946,962,970,972,985,987,998,1003,1005,1014,1015,1016,1059,1060,1086,1102,1103,1104,1111,1112,1113,1121,1122,1129,1131,1133,1134,1135,1153,1154,1164,1167,1168,1169,1170,1171,1172,1173,1180,1186,1187,1188,1192,1193,1194,1195,1196,1197,1198,1214,1216,1236,1237,1238,1257,1258,1266,1270,1271,1272,1273,1298,1299,1305,1309,1310,1311,1312,1317,1318,1326,1330,1339,1342,1343,1347,1348,1349,1368,1370,1372,1373,1374,1375,1381,1382,1383,1387,1389,1393,1395,1399,1401,1412,1415,1416,1417,1482,1490,1492,1493,1503,1504,1505,1506,1507,1521,1522,1543,1544,1549,1550,1552,1553,1563,1564,1566,1567,1568,1569,1570,1595,1597,1598,1599,1650,1651,1652,1657,1659,1660,1661,1662,1704,1710,1711,1719,1720,1721,1740,1745,1746,1747,1749,1750,1751,1752,1754,1762,1763,1764,1765,1770,1771,1816,1817,1818,1823,1824,1825,1828,1829,1830,1853,1854,1855,1856,1857,1859,1867,1868,1872,1931,1933,1935,1936,1958,1959,1971,1972,1988,1998,1999,2000,2001,2005,2006,2007,2011,2012,2013,2028,2030,2032,2041,2042,2043,2044,2051,2052,2053,2063,2064,2065,2072,2106,2108,2109,2123,2129,2130,2132,2133,2134,2135,2160,2170,2178,2179,2180,2181,2206,2238,2247,2269,2296,2309,2310,2311,2330,2331,2332,2334,2343,2345,2346,2358,2361,2362,2363,2368,2370,2398,2399,2400,2440,2441,2446,2447,2448,2452,2463,2464,2475,2502,2503,2508,2509,2510,2511,2512,2520,2530,2567,2572,2573,2576,2584,2585,2612,2614,2615,2616,2621,2625,2626,2627,2628,2630,2631,2632,2635,2648,2650,2651,2652,2653,2654,2660,2661,2662,2664,2665,2666,2667,2669,2671,2680,2692,2699,2700,2701,2703,2713,2714,2715,2719,2720,2721,2726,2727,2733,2745,2746,2759,2778,2786,2796,2797,2798,2803,2804,2842,2843,2844,2845,2849,2851,2863,2864,2865,2866,2867,2868,2869,2870,2871,2872,2891,2892,2893,2919,2921,2941,2942,2964,2967,2968,2969,2970,2982,3004,3005,3006,3008,3009,3010,3012,3013,3014,3015,3016,3017,3018,3019,3037,3038,3039,3079,3080,3081,3094,3118,3120,3121,3122,3123,3130,3131,3134,3144,3145,3146,3176,3177,3178,3179,3195,3196,3197,3198,3200,3209,3211,3236,3238,3244,3245,3254,3255,3259,3260,3261,3262,3276,3277,3296,3297,3309,3322,3323,3328,3329,3337,3340,3368,3380,3381,3382,3386,3387,3401,3409,3410,3411,3412,3450,3452,3507,3508,3523,3524,3525,3532,3533,3542,3558,3559,3568,3569,3575,3605,3606,3607,3610,3611,3624,3629,3639,3645,3646,3654,3655,3688,3694,3695,3738,3739,3740,3741,3745,3746,3747,3751,3760,3761,3762,3763,3773,3802,3817,3818,3819,3820,3840,3844,3880,3881,3907,3908,3909,3913,3914,3923,3931,3932,3933,3942,3963,3964,3965,3983,3989,3990,3991,3992,4013,4014,4019,4020,4022,4023,4024,4026,4028,4029,4065,4076,4077,4078,4135,4136,4137,4138,4143,4150,4170,4171,4180,4189,4197,4198,4200,4201,4206,4208,4209,4211,4212,4213,4224,4237,4286,4287,4288,4296,4346,4348,4355,4391,4392,4411,4412,4413,4414,4429,4433,4434,4451,4453,4454,4455,4472,4505,4508,4513,4514,4519,4535,4537,4549,4550,4551,4555,4556,4557,4558,4562,4568,4576,4584,4586,4587,4617,4619,4629,4631,4632,4653,4657,4696,4698,4700,4701,4715,4718,4727,4728,4729,4732,4733,4734,4740,4757,4758,4765,4767,4772,4773,4774,4775,4786,4787,4800,4801,4802,4803,4804,4805,4806,4815,4817,4843,4844,4845,4846,4848,4854,4862,4863,4864,4870,4871,4900,4901,4902,4903,4906,4918,4919,4922,4928,4929,4935,4943,4947,4948,4949,4967,4972,4973,4976,4979,4980,4981,4986,4987,4988,4991,5002,5003,5004,5006,5007,5010,5012,5013,5014,5015,5016,5040,5053,5054,5055,5091,5092,5102,5103,5111,5115,5118,5119,5123,5134,5149,5150,5151,5173,5175,5176,5177,5178,5179,5180,5181,5182,5183,5190,5191,5244,5245,5255,5258,5259,5263,5265,5297,5298,5326,5327,5330,5331,5336,5338,5364,5366,5370,5371,5372,5373,5374,5375,5379,5391,5393,5396,5397,5399,5400,5401,5411,5412,5413,5459,5463,5464,5489,5518,5519,5520,5522,5524,5531,5532,5539,5550,5551,5555,5556,5557,5583,5584,5585,5594,5595,5614,5619,5620,5621,5639,5651,5653,5655,5656,5657,5661,5662,5668,5681,5682,5683,5684,5687,5688,5689,5697,5700,5701,5702,5703,5707,5708,5713,5724,5725,5737,5738,5750,5766,5767,5782,5783,5784,5785,5790,5805,5806,5810,5826,5836,5857,5858,5860,5882,5883,5903,5904,5905,5907,5908,5909,5910,5918,5919,5921,5922,5928,5930,5931,5932,5936,5937,5953,5958,5960,5961,5982,5988,5989,6016,6025,6026,6061,6062,6086,6087,6095,6096,6097,6103,6104,6108,6109,6114,6116,6119,6121,6129,6156,6157,6158,6171,6172,6173,6179,6180,6201,6225,6227,6228,6229,6232,6233,6234,6246,6247,6248,6268,6269,6270,6271,6282,6283,6285,6289,6290,6309,6310,6341,6359,6362,6367,6369,6370,6371,6372,6380,6381,6400,6401,6402,6422,6431,6445,6446,6483,6487,6488,6492,6498,6500,6506,6507,6510,6511,6519,6520,6521,6522,6523,6524,6525,6543,6552,6553,6559,6560,6561,6564,6565,6567,6579,6585,6586,6587,6595,6596,6599,6614,6615,6616,6619,6627,6631,6632,6633,6634,6635,6637,6638,6645,6651,6652,6653,6661,6662,6669,6670,6694,6705,6707,6708,6709,6714,6718,6722,6723,6742,6743,6744,6746,6747,6763,6764,6765,6766,6767,6783,6784,6785,6786,6801,6809,6810,6816,6817,6846,6858,6873,6874,6875,6876,6880,6881,6885,6895,6896,6897,6908,6917,6919,6927,6929,6930,6931,6932,6942,6944,6945,6954,6955,6956,6962,6966,6995,7033,7034,7037,7043,7044,7045,7049,7063,7064,7065,7071,7072,7073,7074,7084,7090,7092,7093,7096,7097,7098,7099,7104,7118,7127,7128,7133,7174,7194,7195,7197,7198,7199,7208,7209,7213,7237,7238,7239,7240,7253,7254,7255,7259,7279,7342,7347,7348,7349,7350,7352,7354,7355,7357,7358,7368,7385,7395,7429,7430,7431,7432,7490,7498,7529,7534,7537,7543,7544,7545,7546,7560,7562,7563,7564,7568,7569,7572,7584,7590,7602,7627,7628,7634,7636,7641,7642,7643,7644,7645,7657,7658,7659,7660,7661,7666,7668,7671,7676,7680,7681,7682,7699,7703,7706,7709,7710,7714,7715,7718,7719,7721,7722,7724,7727,7730,7732,7754,7769,7771,7775,7785,7786,7810,7811,7812,7842,7843,7844,7850,7858,7859,7869,7873,7877,7890,7891,7892,7893,7894,7895,7896,7900,7913,7930,7931,7932,7933,7934,7938,7939,7940,7941,7945,7951,7960,7961,7966,7970,7971,7972,7973,7974,7975,7977,7980,7981,7982,7983,7985,7999,8003,8004,8005,8012,8013,8015,8017,8047,8053,8064,8065,8083,8084,8087,8093,8094,8095,8097,8115,8116,8117,8120,8123,8124,8125,8126,8129,8131,8132,8135,8136,8143,8144,8146,8147,8148,8185,8188,8205,8206,8211,8212,8213,8214,8215,8225,8229,8230,8252,8255,8256,8257,8258,8260,8293,8294,8295,8305,8306,8307,8332,8333,8334,8335,8338,8342,8343,8344,8345,8365,8366,8367,8368,8373,8374,8379,8380,8384,8410,8426,8427,8428,8429,8442,8443,8444,8445,8462,8463,8464,8465,8467,8477,8482,8483,8484,8485,8499,8500,8501,8502,8503,8552,8553,8554,8565,8604,8605,8612,8626,8637,8638,8639,8644,8647,8648,8649,8659,8665,8676,8677,8680,8681,8683,8687,8688,8689,8696,8697,8725,8726,8727,8740,8742,8743,8750,8752,8753,8758,8768,8784,8785,8786,8787,8788,8796,8803,8821,8830,8831,8842,8849,8859,8860,8881,8883,8892,8893,8900,8901,8902,8912,8923,8924,8925,8941,8942,8943,8945,8946,8947,8948,8949,8962,8963,8989,8994,9006,9007,9010,9011,9015,9017,9030,9057,9058,9060,9061,9065,9067,9096,9099,9103,9118,9119,9120,9121,9122,9130,9134,9141,9154,9155,9164,9168,9169,9170,9179,9181,9194,9208,9210,9211,9212,9213,9220,9223,9226,9235,9263,9264,9266,9267,9268,9269,9286,9310,9311,9312,9325,9332,9350,9352,9355,9356,9358,9361,9374,9376,9377,9380,9381,9385,9389,9400,9408,9409,9420,9424,9430,9438,9445,9446,9460,9461,9470,9473,9483,9495,9496,9500,9501,9503,9512,9516,9529,9530,9536,9542,9555,9560,9561,9565,9566,9567,9577,9582,9583,9587,9589,9591,9592,9626,9628,9629,9634,9635,9640,9641,9642,9646,9647,9648,9649,9650,9652,9658,9661,9662,9664,9665,9668,9669,9676,9678,9679,9680,9682,9687,9688,9689,9695,9696,9703,9730,9740,9741,9742,9743,9748,9749,9751,9752,9753,9763,9779,9780,9798,9805,9806,9814,9821,9822,9823,9824,9831,9837,9838,9848,9854,9858,9859,9860,9862,9872,9874,9899,9900,9905,9906,9925,9926,9928,9932,9938,9945,9950,9953,9961,9965,9966,9967,9972,9976,9977,9988,10015,10016,10035,10038,10039,10042,10043,10044,10045,10054,10055,10057,10058,10060,10063,10065,10073,10074,10080,10095,10096,10097,10099,10103,10104,10113,10121,10122,10124,10125,10132,10143,10148,10168,10172,10173,10177,10181,10182,10187,10204,10205,10216,10220,10230,10236,10254,10255,10266,10267,10269,10270,10297,10298,10304,10305,10306,10311,10312,10313,10317,10327,10328,10331,10342,10343,10373,10378,10394,10395,10396,10403,10408,10409,10430,10432,10434,10435,10470,10471,10479,10504,10517,10542,10543,10544,10556,10557,10559,10560,10565,10567,10568,10569,10571,10572,10584,10589,10605,10607,10613,10615,10626,10627,10633,10638,10642,10643,10644,10649,10655,10656,10657,10664,10665,10670,10671,10677,10687,10688,10690,10699,10700,10701,10723,10724,10725,10727,10730,10734,10738,10739,10757,10758,10759,10761,10762,10771,10774,10775,10776,10777,10780,10783,10784,10787,10788,10807,10808,10815,10823,10824,10834,10855,10867,10874,10879,10889,10911,10912,10913,10914,10915,10916,10918,10919,10924,10925,10928,10932,10933,10937,10942,10946,10948,10949,10953,10954,10964,10977,10978,11002,11006,11007,11008,11015,11021,11022,11026,11027,11032,11060,11067,11068,11069,11075,11080,11082,11083,11084,11090,11105,11107,11110,11111,11112,11115,11116,11120,11130,11145,11146,11155,11165,11183,11192,11193,11197,11201,11202,11209,11210,11211,11217,11219,11222,11231,11232,11234,11249,11251,11252,11269,11292,11294,11298,11327,11330,11331,11338,11344,11345,11346,11347,11349,11350,11364,11365,11366,11367,11386,11387,11389,11390,11404,11410,11412,11415,11416,11417,11430,11436,11437,11455,11456,11457,11465,11466,11467,11476,11480,11483,11492,11503,11506,11519,11520,11521,11524,11527,11528,11532,11533,11534,11535,11543,11545,11553,11554,11560,11579,11583,11603,11604,11606,11611,11630,11641,11651,11656,11667,11668,11683,11685,11686,11687,11695,11704,11709,11710,11711,11713,11721,11723,11724,11725,11726,11727,11743,11745,11747,11748,11756,11769,11787,11792,11803,11804,11836,11841,11842,11848,11849,11860,11861,11903,11911,11914,11915,11917,11918,11925,11929,11938,11939,11951,11952,11953,11955,11956,11969,11970,11979,11985,11986,11991,11992,11994,12007,12008,12009,12010,12026,12048,12059,12060,12069,12070,12071,12079,12098,12099,12109,12126,12127,12132,12143,12146,12148,12149,12155,12171,12173,12174,12175,12187,12191,12197,12212,12213,12214,12219,12227,12233,12252,12267,12278,12285,12286,12287,12291,12294,12295,12296,12297,12299,12306,12317,12321,12322,12324,12330,12331,12333,12341,12342,12343,12344,12357,12364,12369,12374,12383,12384,12390,12397,12398,12399,12400,12402,12411,12412,12433,12449,12467,12468,12469,12495,12505,12511,12513,12515,12516,12517,12530,12531,12539,12540,12541,12549,12550,12553,12560,12566,12567,12582,12586,12598,12601,12602,12603,12606,12612,12613,12614,12615,12619,12628,12636,12639,12645,12650,12651,12659,12701,12715,12716,12717,12732,12738,12740,12757,12758,12761,12778,12779,12780,12781,12800,12802,12810,12811,12812,12816,12827,12832,12833,12841,12842,12843,12882,12883,12884,12885,12887,12888,12895,12905,12917,12926,12927,12928,12929,12935,12940,12945,12952,12960,12981,12984,12987,12991,12994,13012,13014,13021,13022,13043,13044,13045,13051,13054,13055,13056,13057,13059,13064,13067,13068,13069,13070,13081,13083,13088,13090,13092,13095,13102,13105,13106,13112,13113,13120,13127,13132,13137,13138,13139,13140,13144,13145,13146,13147,13157,13163,13188,13192,13196,13199,13200,13210,13234,13239,13247,13248,13252,13261,13263,13264,13269,13272,13273,13276,13282,13284,13288,13307,13323,13325,13326,13331,13332,13333,13339,13341,13342,13343,13344,13346,13347,13348,13349,13350,13358,13361,13364,13366,13367,13369,13370,13371,13374,13379,13380,13396,13397,13406,13415,13420,13421,13422,13424,13426,13428,13429,13440,13442,13443,13453,13458,13462,13467,13468,13474,13475,13487,13488,13499,13503,13510,13520,13528,13530,13537,13548,13549,13550,13563,13564,13571,13572,13574,13576,13579,13591,13595,13597,13622,13626,13629,13630,13631,13649,13657,13666,13667,13672,13673,13679,13682,13683,13694,13696,13706,13707,13721,13725,13731,13732,13738,13739,13740,13742,13743,13748,13753,13763,13767,13771,13783,13789,13790,13791,13796,13799,13803,13804,13805,13806,13810,13815,13817,13818,13821,13822,13833,13837,13842,13843,13855,13856,13857,13863,13867,13872,13873,13874,13875,13876,13877,13878,13887,13888,13889,13896,13905,13906,13916,13918,13929,13933,13939,13942,13944,13945,13946,13947,13948,13955,13958,13959,13973,13984,13985,13986,13997,14001,14002,14005,14007,14008,14009,14020,14036,14051,14055,14056,14057,14064,14065,14070,14077,14093,14097,14099,14113,14114,14116,14117,14126,14129,14130,14133,14139,14140,14141,14142,14146,14154,14164,14165,14166,14169,14178,14181,14189,14197,14198,14213,14222,14226,14233,14238,14239,14245,14246,14251,14252,14262,14263,14267,14270,14273,14294,14299,14304,14319,14325,14337,14338,14343,14345,14347,14353,14360,14361,14362,14368,14369,14382,14387,14388,14396,14397,14409,14411,14415,14424,14425,14445,14449,14460,14462,14464,14466,14468,14475,14482,14483,14485,14495,14496,14503,14504,14515,14525,14526,14538,14545,14546,14547,14548,14549,14560,14561,14562,14563,14566,14571,14577,14596,14597,14598,14614,14628,14640,14644,14648,14652,14654,14658,14660,14662,14667,14668,14669,14670,14685,14694,14700,14701,14702,14707,14716,14721,14722,14726,14740,14741,14742,14744,14755,14758,14780,14781,14787,14793,14794,14796,14797,14801,14815,14816,14824,14830,14846,14848,14864,14865,14879,14881,14883,14884,14903,14905,14926,14928,14932,14933,14934,14935,14936,14961,14965,14967,14973,14975,14983,14990,14999,15000,15004,15005,15006,15007,15022,15023,15029,15036,15037,15038,15045,15047,15054,15061,15072,15089,15090,15101,15114,15118,15128,15129,15132,15141,15148,15149,15150,15158,15160,15161,15164,15165,15168,15172,15173,15174,15183,15191,15194,15196,15200,15209,15210,15213,15216,15217,15221,15228,15229,15243,15247,15286,15287,15291,15321,15322,15331,15352,15353,15354,15360,15362,15363,15377,15379,15384,15393,15394,15395,15397,15403,15406,15412,15422,15451,15455,15470,15473,15476,15478,15482,15487,15492,15498,15501,15504,15522,15552,15556,15574,15576,15581,15585,15586,15587,15602,15603,15604,15605,15615,15616,15617,15624,15625,15637,15644,15646,15662,15663,15664,15682,15688,15689,15692,15726,15735,15740,15743,15756,15758,15763,15796,15802,15803,15805,15809,15810,15811,15818,15823,15827,15836,15839,15841};
  on_feature_edge.reset();
  for(int i=0;i<2746;++i)
    on_feature_edge.set(indices_with_cov[i]);
  nb_wrong=0;
  for (std::size_t i=0; i<points.size(); ++i)
    if( on_feature_edge[i] != CGAL::vcm_is_on_feature_edge(cov[i], 0.18)) ++nb_wrong;
  assert( nb_wrong < 20 ); // be tolerant to numeric errors

  return true;
}

int main ()
{
  bool res1=test_fandisk();
  if (res1)
    std::cout << "fandisk tests OK\n";
  else
    std::cout << "fandisk tests KO\n";

  bool res2 = true;

  if (!res1 || !res2) return 1;
  return 0;
}
