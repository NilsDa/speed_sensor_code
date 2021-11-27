
#include "math.h"
//#include "mem_manager.h"


static const uint16_t ATAN_LUT[90] = {0,1,3,5,7,8,10,11,14,16,17,19,20,23,25,26,28,30,32,34,36,38,40,42,44,46,49,51,53,55,58,59,62,64,67,70,72,75,78,80,84,86,90,93,96,100,103,106,110,115,118,123,128,132,137,142,147,155,161,167,171,179,186,194,203,212,223,234,245,258,276,292,310,327,348,372,396,427,476,520,568,625,700,840,969,1154,1411,1814,3175,6350};
static const uint16_t ATAN_LUT0[900] = { 0, 1, 3, 4, 6, 8, 10, 12, 13, 15, 17, 19, 20, 22, 24, 26, 28, 29, 31, 33, 35, 37, 38, 40, 42, 43, 44, 46, 48, 50, 52, 53, 55, 57, 59, 61, 62, 64, 66, 68, 69, 71, 73, 75, 77, 78, 80, 82, 84, 85, 87, 89, 91, 93, 94, 96, 98, 100, 101, 103, 105, 107, 109, 110, 111, 114, 116, 117, 118, 120, 123, 125, 125, 127, 130, 132, 133, 134, 136, 139, 141, 142, 143, 146, 148, 149, 151, 153, 155, 156, 158, 160, 162, 163, 165, 167, 169, 171, 172, 174, 176, 178, 179, 181, 183, 185, 186, 188, 190, 192, 194, 196, 198, 200, 201, 203, 205, 207, 208, 210, 212, 214, 215, 218, 220, 222, 223, 225, 227, 229, 230, 232, 234, 236, 237, 239, 242, 244, 245, 247, 249, 250, 252, 255, 257, 258, 260, 262, 264, 265, 267, 269, 271, 273, 275, 277, 278, 280, 283, 285, 286, 288, 290, 291, 294, 296, 298, 299, 302, 304, 305, 307, 309, 311, 313, 315, 317, 318, 321, 323, 324, 326, 328, 331, 332, 334, 336, 338, 340, 342, 344, 346, 348, 349, 351, 354, 356, 357, 360, 362, 363, 365, 368, 369, 371, 374, 375, 377, 380, 381, 383, 386, 387, 389, 392, 393, 396, 398, 399, 402, 404, 405, 408, 410, 411, 414, 416, 417, 420, 422, 424, 426, 428, 430, 432, 434, 437, 439, 440, 443, 444, 447, 449, 451, 453, 456, 457, 460, 462, 463, 466, 468, 470, 473, 474, 477, 478, 481, 483, 485, 488, 489, 491, 494, 496, 498, 501, 502, 505, 507, 509, 510, 513, 516, 518, 520, 522, 524, 527, 529, 532, 533, 536, 537, 540, 543, 545, 547, 549, 552, 553, 556, 559, 561, 563, 565, 568, 570, 572, 574, 577, 580, 581, 584, 586, 589, 591, 594, 595, 598, 600, 603, 605, 608, 610, 612, 615, 617, 620, 622, 625, 627, 629, 631, 634, 636, 639, 641, 644, 646, 649, 651, 654, 656, 659, 661, 664, 666, 669, 671, 674, 676, 680, 682, 685, 687, 689, 692, 694, 697, 699, 702, 704, 708, 710, 713, 715, 718, 720, 723, 727, 729, 732, 734, 737, 739, 743, 745, 747, 750, 753, 756, 758, 761, 765, 767, 770, 772, 776, 778, 780, 784, 786, 789, 792, 795, 798, 801, 804, 806, 809, 812, 814, 819, 821, 824, 827, 829, 832, 835, 839, 842, 845, 847, 851, 853, 857, 860, 863, 865, 869, 871, 875, 879, 881, 885, 887, 890, 895, 897, 900, 903, 906, 910, 912, 916, 919, 921, 926, 929, 931, 935, 939, 942, 946, 948, 951, 955, 959, 962, 964, 970, 972, 975, 978, 983, 986, 989, 993, 997, 1000, 1002, 1006, 1011, 1013, 1016, 1022, 1025, 1028, 1030, 1036, 1039, 1042, 1046, 1050, 1053, 1056, 1061, 1064, 1068, 1073, 1076, 1079, 1085, 1088, 1091, 1095, 1098, 1103, 1106, 1110, 1114, 1117, 1123, 1126, 1129, 1134, 1137, 1142, 1147, 1150, 1155, 1158, 1161, 1166, 1171, 1174, 1179, 1183, 1186, 1191, 1196, 1201, 1204, 1208, 1213, 1216, 1220, 1227, 1230, 1236, 1239, 1242, 1248, 1251, 1257, 1262, 1266, 1271, 1275, 1280, 1284, 1288, 1293, 1297, 1303, 1307, 1312, 1318, 1322, 1327, 1331, 1337, 1341, 1345, 1351, 1355, 1361, 1365, 1371, 1375, 1381, 1387, 1391, 1397, 1401, 1408, 1412, 1418, 1422, 1429, 1433, 1440, 1444, 1450, 1455, 1459, 1466, 1470, 1477, 1481, 1488, 1492, 1500, 1504, 1511, 1515, 1523, 1527, 1534, 1539, 1546, 1551, 1558, 1563, 1570, 1575, 1583, 1587, 1593, 1598, 1606, 1611, 1618, 1623, 1631, 1636, 1644, 1652, 1657, 1666, 1671, 1679, 1682, 1690, 1696, 1704, 1709, 1718, 1723, 1732, 1741, 1746, 1753, 1758, 1767, 1773, 1782, 1788, 1797, 1806, 1810, 1819, 1825, 1834, 1840, 1850, 1859, 1863, 1873, 1879, 1889, 1895, 1905, 1913, 1919, 1929, 1936, 1946, 1957, 1961, 1971, 1978, 1989, 1995, 2004, 2015, 2022, 2033, 2044, 2048, 2060, 2067, 2078, 2088, 2095, 2107, 2114, 2126, 2135, 2143, 2155, 2162, 2172, 2185, 2192, 2205, 2216, 2223, 2236, 2247, 2255, 2268, 2276, 2287, 2301, 2309, 2320, 2334, 2343, 2357, 2369, 2377, 2392, 2398, 2413, 2428, 2434, 2449, 2464, 2471, 2486, 2502, 2509, 2525, 2541, 2548, 2564, 2581, 2588, 2604, 2621, 2629, 2646, 2663, 2671, 2689, 2704, 2714, 2732, 2748, 2759, 2775, 2794, 2805, 2821, 2841, 2857, 2869, 2889, 2906, 2918, 2936, 2957, 2969, 2987, 3009, 3018, 3040, 3059, 3082, 3092, 3115, 3138, 3148, 3172, 3192, 3206, 3227, 3252, 3274, 3288, 3310, 3336, 3348, 3374, 3397, 3425, 3437, 3464, 3489, 3505, 3530, 3559, 3585, 3602, 3628, 3659, 3686, 3704, 3732, 3764, 3778, 3807, 3841, 3871, 3890, 3920, 3956, 3987, 4008, 4040, 4073, 4094, 4128, 4167, 4202, 4224, 4260, 4297, 4339, 4358, 4400, 4440, 4479, 4504, 4545, 4587, 4634, 4655, 4704, 4748, 4794, 4822, 4868, 4916, 4970, 4995, 5045, 5101, 5153, 5185, 5239, 5294, 5356, 5385, 5443, 5502, 5569, 5600, 5662, 5732, 5798, 5832, 5906, 5976, 6047, 6127, 6164, 6240, 6318, 6405, 6445, 6529, 6614, 6708, 6753, 6844, 6945, 7041, 7090, 7191, 7302, 7408, 7518, 7574, 7689, 7815, 7937, 8000, 8128, 8268, 8404, 8546, 8618, 8767, 8929, 9089, 9171, 9339, 9514, 9704, 9893, 9990, 10190, 10397, 10614, 10736, 10967, 11208, 11460, 11724, 11860, 12154, 12451, 12762, 12924, 13259, 13613, 13986, 14394, 14600, 15029, 15484, 15968, 16222, 16754, 17322, 17929, 18600, 18944, 19673, 20460, 21312, 22239, 22733, 23790, 24951, 26230, 26921, 28416, 30088, 31968, 34133, 35310, 37925, 40960, 44521, 48761, 51200, 56888, 64000, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535 };

	
int16_t mean(int16_t *arr, uint16_t length)
{
	int32_t sum = 0;
	for (int16_t i = 0; i < length; i++)
	{
		sum += arr[i];
	}
	return (int16_t) (sum / length);
}

int32_t variance(int16_t *arr, int16_t m, uint16_t length)
{
	int32_t sum = 0;
	for (int16_t i = 0; i < length; i++)
	{
		sum += (arr[i] - m) * (arr[i] - m);
	}
	return (int32_t) (sum / (length - 1));
}

int16_t max(int16_t *arr, uint16_t length)
{
	int16_t mx = arr[0];
	for (int16_t i = 1; i < length; i++)
	{
		if(arr[i] > mx)
			mx = arr[i];
	}
	return mx;
}

int16_t min(int16_t *arr, uint16_t length)
{
	int16_t mn = arr[0];
	for (int16_t i = 1; i < length; i++)
	{
		if(arr[i] < mn)
			mn = arr[i];
	}
	return mn;
}


/////////////////////////////////////////////////////////////////////////////////////////
void ma_filter_init(ma_filter_t* fltr, uint8_t len)
{
	fltr->length = len;
	fltr->data = (int16_t*) calloc(len, sizeof(int16_t));
	fltr->sum = 0;
	fltr->current_index = 0;
	fltr->filled_once = false;
	
	return;
}
void ma_filter_uninit(ma_filter_t* fltr)
{
	free(fltr->data);
}

int16_t ma_filter_step(ma_filter_t* fltr, int16_t val)
{
  if(fltr->filled_once)
  {
    fltr->sum += (int32_t)val - (int32_t)(fltr->data[fltr->current_index]);
	/*if(fltr->sum < 0)
		fltr->sum = 0;*/
    fltr->data[fltr->current_index] = val;
  }
  else
  {
    fltr->data[fltr->current_index] = val;
	fltr->sum = fltr->sum + val;
  }
  
  fltr->current_index++;
  if(fltr->current_index >= fltr->length)
  {
    fltr->current_index = 0;
    if(!fltr->filled_once)
      fltr->filled_once = true;
  }

  return (int16_t)(fltr->sum / fltr->length);
}

void ma_filter_reset(ma_filter_t* fltr)
{
	fltr->sum = 0;
	fltr->current_index = 0;
	fltr->filled_once = false;
}
/////////////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////////////////
#pragma push
#pragma O0  // do not optimize these functions -> Optimization level 0
bool RANKFILTER_Init(sRankFilter_t *rankFilter, int16_t nodeCount)
{
    if(rankFilter && rankFilter->rankBuffer &&
        (nodeCount % 2) && (nodeCount > 1) &&
        (rankFilter->numNodesMax % 2) && (rankFilter->numNodesMax > 1) &&
		(rankFilter->numNodesMax >= nodeCount))
    {
        //initialize buffer nodes
        for(uint16_t i = 0; i < rankFilter->numNodesMax; i++)
        {
            rankFilter->rankBuffer[i].value = 0;
            rankFilter->rankBuffer[i].nextAge = &rankFilter->rankBuffer[(i + 1) % rankFilter->numNodesMax];
            rankFilter->rankBuffer[i].nextValue = &rankFilter->rankBuffer[(i + 1) % rankFilter->numNodesMax];
            rankFilter->rankBuffer[(i + 1) % rankFilter->numNodesMax].prevValue = &rankFilter->rankBuffer[i];
            rankFilter->rankBuffer[(i + 1) % rankFilter->numNodesMax].prevAge = &rankFilter->rankBuffer[i];
        }
        //initialize heads
        rankFilter->ageTail = rankFilter->rankBuffer;
        rankFilter->valueHead = rankFilter->rankBuffer;
        rankFilter->valueMedian = &rankFilter->rankBuffer[nodeCount / 2];
        rankFilter->valueTail = &rankFilter->rankBuffer[nodeCount - 1];
        rankFilter->numNodes = rankFilter->numNodesMax;
        
        if(rankFilter->numNodesMax > nodeCount)
        {
            rankFilter->unusedHead = &rankFilter->rankBuffer[nodeCount];
            RANKFILTER_UpdateNumNodes(rankFilter, nodeCount);
        }
        else
        {
            rankFilter->unusedHead = &rankFilter->rankBuffer[nodeCount - 1];
        }
        
        return true;
    }

    return false;
}


int16_t* RANKFILTER_Step(sRankFilter_t *rankFilter, int16_t sample)
{
	static int16_t ret_val[3] = { 0, 0, 0 };
	
    uint16_t i;
    // volatile sRankNode_t *newNode, *it;

    if(rankFilter->ageTail == rankFilter->valueHead)
    {   //if oldest node is also the smallest node, increment value head
        rankFilter->valueHead = rankFilter->valueHead->nextValue;
    }

    if((rankFilter->ageTail == rankFilter->valueMedian) ||
        (rankFilter->ageTail->value > rankFilter->valueMedian->value))
    {   //prepare for median correction
        rankFilter->valueMedian = rankFilter->valueMedian->prevValue;
    }
	
    if(rankFilter->ageTail == rankFilter->valueTail)
    {   //prepare for max correction
        rankFilter->valueTail = rankFilter->valueTail->prevValue;
    }

    //replace age tail with new sample
    sRankNode_t* newNode = rankFilter->ageTail;
    newNode->value = sample;

    //remove age tail from list
    rankFilter->ageTail->nextValue->prevValue = rankFilter->ageTail->prevValue;
    rankFilter->ageTail->prevValue->nextValue = rankFilter->ageTail->nextValue;
    //increment age tail
    rankFilter->ageTail = rankFilter->ageTail->nextAge;

    //find new node position
    sRankNode_t* it = rankFilter->valueHead; //set iterator as value head
    for(i = 0; i < rankFilter->numNodes - 1; i++)
    {		
        if(sample < it->value)
        {
            if(i == 0)
            {   //replace value head if new node is the smallest
                rankFilter->valueHead = newNode;
            }
            break;
        }
        it = it->nextValue;
    }
	
	// replace value tail if new node is the largest
	if(sample >= it->value)
		rankFilter->valueTail = newNode;

    //insert new node in list
    it->prevValue->nextValue = newNode;
    newNode->prevValue = it->prevValue;
    it->prevValue = newNode;
    newNode->nextValue = it;

    //adjust median node
    if(i >= (rankFilter->numNodes / 2))
    {
        rankFilter->valueMedian = rankFilter->valueMedian->nextValue;
    }

	ret_val[0] = rankFilter->valueHead->value;
	ret_val[1] = rankFilter->valueMedian->value;
	ret_val[2] = rankFilter->valueTail->value;
	
    return ret_val;
}

void RANKFILTER_UpdateNumNodes(sRankFilter_t *rankFilter, int16_t count)
{
	if(count == rankFilter->numNodes || count < 3 || ((count - 1) % 2))
		return;
	
	if(count > rankFilter->numNodesMax)
		count = rankFilter->numNodesMax;
	
	if(count < rankFilter->numNodes)
	{
	    // temporarily save head
		sRankNode_t *ageHead = rankFilter->ageTail->prevAge;
		//printf("ageHead: %d  ageHead->nextAge: %d\n", ageHead->value, ageHead->nextAge->value);
		rankFilter->unusedHead = rankFilter->ageTail;
		sRankNode_t *it = rankFilter->ageTail;
		uint16_t i = 0;
		// int16_t tmp = 100;
		for(i = 0; i < rankFilter->numNodes - count; i++)
		{
		    if(it == rankFilter->valueHead)
		        rankFilter->valueHead = rankFilter->valueHead->nextValue;
		    if(it == rankFilter->valueTail)
		        rankFilter->valueTail = rankFilter->valueTail->prevValue;
		    
		    it->prevValue->nextValue = it->nextValue;
			it->nextValue->prevValue = it->prevValue;
			//it->value = tmp++;  // reset value
			it->value = 0;  // reset value
			
			it = it->nextAge;
		}
		rankFilter->ageTail = it;
		rankFilter->ageTail->prevAge = ageHead;
		ageHead->nextAge = rankFilter->ageTail;
	}
	else
	{
	    uint16_t i;
	    
		// temporarily save age head and age tail
		sRankNode_t *oldAgeHead = rankFilter->ageTail->prevAge;
		sRankNode_t *oldAgeTail = rankFilter->ageTail;
		// "reattach" oldest count-numNodes nodes to filter rank
		rankFilter->ageTail = rankFilter->unusedHead;
		rankFilter->ageTail->prevAge = oldAgeHead;
		oldAgeHead->nextAge = rankFilter->ageTail;
		//find position to reattach nodes with value 0
		sRankNode_t *pos_reattach = rankFilter->valueHead;
		uint16_t updateValueHead = 0;
		for(i = 0; i < rankFilter->numNodes; i++)
		{
		  if(pos_reattach->value >= 0)
		    break;
			pos_reattach = pos_reattach->nextValue;
		}
		if(i == 0)
		    updateValueHead = 1;
		sRankNode_t *it = rankFilter->unusedHead;
		for(i = 0; i < count - rankFilter->numNodes; i++)
		{
			it->prevValue = pos_reattach->prevValue;
			it->prevValue->nextValue = it;
			pos_reattach->prevValue = it;
			it->nextValue = pos_reattach;
			it->value = 0;
			
			if(i == count - rankFilter->numNodes - 1)
			    break;
			
			pos_reattach = it;
			it = it->nextAge;
		}
		
		oldAgeTail->prevAge = it;
		it->nextAge = oldAgeTail;
		
		if(updateValueHead)
		    rankFilter->valueHead = it;
	}
	
	//update value median
	sRankNode_t *it = rankFilter->valueHead;
  for(int16_t i = 0; i < (count - 1) / 2; i++)
    it = it->nextValue;
	rankFilter->valueMedian = it;
	
	rankFilter->numNodes = count;
	
	return;
}
#pragma pop  // restore normal optimization level
/////////////////////////////////////////////////////////////////////////////////////////


uint16_t atan2(int16_t x, int16_t y)
{
  if(x == 0)
  {
    if(y > 0)
      return 90;
    else
      return 270;
  }

  int32_t ratio = (int32_t)100*((double)abs(y) / (double)abs(x));

  uint8_t i = 0;
//  while(ratio > ATAN_LUT[i] && i < 90)
  while(ratio > ATAN_LUT[i] && i < 89)
    i++;

  // I
  uint16_t angle = i;

  if(x < 0)
  {
    if(y >= 0) // II
    {
      angle = 180 - i;
    }
    else //III
    {
      angle = 180 + i;
    }
  }
  else if(y < 0) // IV
  {
    angle = 360 - i;
  }
	
	return angle;
}

uint16_t atan20(int16_t x, int16_t y)
{
  if(x == 0)
  {
    if(y > 0)
      return 900;
    else
      return 2700;
  }

  int32_t ratio = (int32_t)1000*((double)abs(y) / (double)abs(x));

  uint16_t i = 0;
  // while(ratio > ATAN_LUT0[i] && i < 900)
  while(ratio > ATAN_LUT0[i] && i < 899)
    i++;

  // I
  uint16_t angle = i;

  if(x < 0)
  {
    if(y >= 0) // II
    {
      angle = 1800 - i;
    }
    else //III
    {
      angle = 1800 + i;
    }
  }
  else if(y < 0) // IV
  {
    angle = 3600 - i;
  }
	
	return angle;
}


