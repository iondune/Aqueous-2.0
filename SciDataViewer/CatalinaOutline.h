
#pragma once

#include <ionMath.h>


static vector<vec2f> const CatalinaOutline =
{
	vec2d(33.40287270967907,-118.48342836119),
	vec2d(33.38168206583182,-118.4753000364343),
	vec2d(33.36994142606682,-118.4810840567663),
	vec2d(33.3537779726197,-118.4892248274207),
	vec2d(33.33464873296537,-118.4744342492724),
	vec2d(33.32061793125764,-118.4411204795906),
	vec2d(33.32032879047411,-118.4071292564062),
	vec2d(33.31384842896512,-118.3566048789315),
	vec2d(33.3018283788155,-118.3289082402636),
	vec2d(33.30569558056143,-118.3114905594644),
	vec2d(33.32913711920763,-118.3068109305513),
	vec2d(33.3628032759772,-118.3432057375471),
	vec2d(33.39038804566135,-118.3671047318737),
	vec2d(33.42209847251463,-118.405761478115),
	vec2d(33.42137400944346,-118.4089356327479),
	vec2d(33.42390758440389,-118.4159175636193),
	vec2d(33.42676860858607,-118.4198896983067),
	vec2d(33.42796912257573,-118.4238562645845),
	vec2d(33.42696210138514,-118.4256397051392),
	vec2d(33.42778613672946,-118.4283557393787),
	vec2d(33.42745801722527,-118.4309678677934),
	vec2d(33.4266532693571,-118.4324259720863),
	vec2d(33.42646506755799,-118.4344228797274),
	vec2d(33.42815524974935,-118.4355210456894),
	vec2d(33.42846215416081,-118.4375223562103),
	vec2d(33.42832767497746,-118.4401933501179),
	vec2d(33.42815366597858,-118.4437764327807),
	vec2d(33.42966840433842,-118.4469310133317),
	vec2d(33.43254407146148,-118.4493938361489),
	vec2d(33.43316085718386,-118.4540947551123),
	vec2d(33.43658492690436,-118.4580365697239),
	vec2d(33.43809840650082,-118.4617281350344),
	vec2d(33.44003474156013,-118.4627029505352),
	vec2d(33.44144964321501,-118.4670806223846),
	vec2d(33.44252914844518,-118.4687427518239),
	vec2d(33.44200691942499,-118.4718613785841),
	vec2d(33.44465785230082,-118.4750698677735),
	vec2d(33.44834002332183,-118.4775477453084),
	vec2d(33.44810636660532,-118.4783819143154),
	vec2d(33.44806761247838,-118.4785775689124),
	vec2d(33.4477937334518,-118.4789963949037),
	vec2d(33.4476920633582,-118.4793451852713),
	vec2d(33.44770692721881,-118.4797214319878),
	vec2d(33.44776731238921,-118.4799084642095),
	vec2d(33.44798894896115,-118.4799804051486),
	vec2d(33.4479796134581,-118.4801726502554),
	vec2d(33.44790398475036,-118.480560435121),
	vec2d(33.44785778745821,-118.4808794231339),
	vec2d(33.44788792092245,-118.4812571882797),
	vec2d(33.44785232411206,-118.4815362117293),
	vec2d(33.44776557353213,-118.4818454547979),
	vec2d(33.4477595767411,-118.4821103797989),
	vec2d(33.4476484822489,-118.4824807439247),
	vec2d(33.44752734039633,-118.4827662711084),
	vec2d(33.44741714533105,-118.4829936262709),
	vec2d(33.44732035208503,-118.4830903810356),
	vec2d(33.44725977835957,-118.4832821169294),
	vec2d(33.44727091731455,-118.4834982843019),
	vec2d(33.44727686307785,-118.4836009950301),
	vec2d(33.44726975864914,-118.4837300752095),
	vec2d(33.44720317560173,-118.4839467361376),
	vec2d(33.44718301190034,-118.4840249381745),
	vec2d(33.44714352435197,-118.4839503631041),
	vec2d(33.44708121927341,-118.4839508873577),
	vec2d(33.44695800264175,-118.4841397806577),
	vec2d(33.44697295786564,-118.4842632865836),
	vec2d(33.4469496854224,-118.4843638765884),
	vec2d(33.44696783932029,-118.484535028337),
	vec2d(33.44682295490355,-118.4846755884794),
	vec2d(33.44669037446818,-118.4847445874882),
	vec2d(33.44667050389752,-118.484886513743),
	vec2d(33.4466200158429,-118.4850779310043),
	vec2d(33.44656187016662,-118.485186638435),
	vec2d(33.446509338068,-118.4852982889264),
	vec2d(33.44643851279321,-118.485338261014),
	vec2d(33.44626392586152,-118.4853826254869),
	vec2d(33.44613081358752,-118.4852811686883),
	vec2d(33.44588935122831,-118.4850845776691),
	vec2d(33.4457215327691,-118.4848996719811),
	vec2d(33.44565489707352,-118.4848144671935),
	vec2d(33.44554878828348,-118.4848137887502),
	vec2d(33.44542007842873,-118.4846623600954),
	vec2d(33.4453572702957,-118.4844819657575),
	vec2d(33.44540106835964,-118.4844232371085),
	vec2d(33.44537216025273,-118.4843515885985),
	vec2d(33.44532816424166,-118.4841772907618),
	vec2d(33.44527700488116,-118.4839939695205),
	vec2d(33.44529683418411,-118.4838765626215),
	vec2d(33.44527624786402,-118.4837832464687),
	vec2d(33.44520768516738,-118.4837239464746),
	vec2d(33.44516168457844,-118.4835779487255),
	vec2d(33.44513892311937,-118.4834520049775),
	vec2d(33.44509044209782,-118.4833604957408),
	vec2d(33.44501245060783,-118.4833090001048),
	vec2d(33.4449367837977,-118.4834445591613),
	vec2d(33.44496711972298,-118.4835522658698),
	vec2d(33.44492737752012,-118.4837074721886),
	vec2d(33.44498992674802,-118.4839370771828),
	vec2d(33.44500139907739,-118.484047731668),
	vec2d(33.4449461970025,-118.4841336364077),
	vec2d(33.44489789078028,-118.4841393232492),
	vec2d(33.44482370045271,-118.4840339382765),
	vec2d(33.4447909706165,-118.4838927351882),
	vec2d(33.44474024301375,-118.4837683862513),
	vec2d(33.44464131172211,-118.4837544280838),
	vec2d(33.44452814736477,-118.4838485377148),
	vec2d(33.44425098575307,-118.4841170370005),
	vec2d(33.44402924437263,-118.4842748266681),
	vec2d(33.44380511042184,-118.4845406516821),
	vec2d(33.44369172339327,-118.4847020993126),
	vec2d(33.4437288516389,-118.4847925185223),
	vec2d(33.44377451879252,-118.4848400334666),
	vec2d(33.4436698071296,-118.4850071417424),
	vec2d(33.44356745426941,-118.4851437824585),
	vec2d(33.44346859423042,-118.4851813416644),
	vec2d(33.44347547311448,-118.4852410875381),
	vec2d(33.44352866654906,-118.4852844794044),
	vec2d(33.44360430040882,-118.4853894930043),
	vec2d(33.44362987347981,-118.4856639496961),
	vec2d(33.44350388377254,-118.4858006727903),
	vec2d(33.44341939714974,-118.4859024555891),
	vec2d(33.44344259271855,-118.4859525954467),
	vec2d(33.44349912347018,-118.485903743652),
	vec2d(33.4435965243471,-118.4858635284402),
	vec2d(33.44365672602253,-118.4858625234822),
	vec2d(33.44369170831217,-118.4859136732187),
	vec2d(33.44376780784288,-118.4859092443628),
	vec2d(33.44379392654382,-118.4858508925245),
	vec2d(33.44391835054332,-118.4858886962246),
	vec2d(33.44384857797513,-118.4859630851234),
	vec2d(33.4438689777519,-118.4860532094846),
	vec2d(33.443874042892,-118.4861666108843),
	vec2d(33.44393206566129,-118.4862052125799),
	vec2d(33.4440775301324,-118.4862154662937),
	vec2d(33.4441121123776,-118.4862571104563),
	vec2d(33.44402443349075,-118.4862486386116),
	vec2d(33.44396508061933,-118.4863054181962),
	vec2d(33.44398671870975,-118.4864142504397),
	vec2d(33.44407081959421,-118.48645589504),
	vec2d(33.44414031507983,-118.4865300322441),
	vec2d(33.44418833679758,-118.4866153208903),
	vec2d(33.44426876295666,-118.4867603364035),
	vec2d(33.44430487201494,-118.4869151810595),
	vec2d(33.44432987231887,-118.4870443014927),
	vec2d(33.44429805050638,-118.4871790837116),
	vec2d(33.44436636652679,-118.4872803536563),
	vec2d(33.44427370087023,-118.4874059033183),
	vec2d(33.44420335640921,-118.4874158986121),
	vec2d(33.44426116883348,-118.4875480688322),
	vec2d(33.44451659918617,-118.4876823696226),
	vec2d(33.44468023358536,-118.4878697088658),
	vec2d(33.44468358833371,-118.4880373769331),
	vec2d(33.44457716048267,-118.4882313311886),
	vec2d(33.44443815001245,-118.4883384710869),
	vec2d(33.44430272500827,-118.4882437677919),
	vec2d(33.44411771019038,-118.4880652417646),
	vec2d(33.44401452773975,-118.4881825837942),
	vec2d(33.44396835594471,-118.4884819651079),
	vec2d(33.44409984304608,-118.4886620584291),
	vec2d(33.44414507636828,-118.4888957070242),
	vec2d(33.44412997920102,-118.4892773933074),
	vec2d(33.44419672575636,-118.4894947134711),
	vec2d(33.4440854419296,-118.4897537019338),
	vec2d(33.44384129690799,-118.4900081354997),
	vec2d(33.44366671409921,-118.4901174964522),
	vec2d(33.44356266283803,-118.4902792814498),
	vec2d(33.44337838026878,-118.4903141921663),
	vec2d(33.44333533495181,-118.4905477779281),
	vec2d(33.44342594716891,-118.4906583523355),
	vec2d(33.44352216753583,-118.4909035297888),
	vec2d(33.44330318834287,-118.4910555825442),
	vec2d(33.44309130780844,-118.4911938255632),
	vec2d(33.44287253587405,-118.4913061550576),
	vec2d(33.44257323901503,-118.4910764175033),
	vec2d(33.44238021010649,-118.4908989141018),
	vec2d(33.44219475439805,-118.4911063780193),
	vec2d(33.44203481316671,-118.4914005692643),
	vec2d(33.44195208298066,-118.4917854291079),
	vec2d(33.44193519701223,-118.4922367297731),
	vec2d(33.44189870105063,-118.4924693937962),
	vec2d(33.44177654251721,-118.4926807698461),
	vec2d(33.44157948645272,-118.4929230856963),
	vec2d(33.44138603908321,-118.4931275642272),
	vec2d(33.4412159014672,-118.4935051347515),
	vec2d(33.44114525275793,-118.4937952472242),
	vec2d(33.44113760956738,-118.4941900171692),
	vec2d(33.44112045258916,-118.4945563879816),
	vec2d(33.44113702811336,-118.494968045885),
	vec2d(33.441176239816,-118.4951877755317),
	vec2d(33.44124854339857,-118.4953379051458),
	vec2d(33.44124297115667,-118.4954812025426),
	vec2d(33.4412072914809,-118.4956358992096),
	vec2d(33.44123666193128,-118.4957351804676),
	vec2d(33.44128426181956,-118.4957674281211),
	vec2d(33.44124264929717,-118.4958490780322),
	vec2d(33.44122339293057,-118.4959624107464),
	vec2d(33.441210897695,-118.4961449391892),
	vec2d(33.4411937045432,-118.496354380716),
	vec2d(33.4412701311223,-118.496745194152),
	vec2d(33.44141712463164,-118.4972562945246),
	vec2d(33.44161578886381,-118.497787877535),
	vec2d(33.44177624488731,-118.498161655018),
	vec2d(33.44215157894514,-118.4986932531431),
	vec2d(33.44340226101083,-118.4998690850425),
	vec2d(33.44433461001273,-118.4989497521837),
	vec2d(33.44514766969817,-118.4983422887135),
	vec2d(33.44592780654258,-118.4982931699945),
	vec2d(33.44653312843532,-118.4993613644814),
	vec2d(33.44688728796331,-118.5001790395292),
	vec2d(33.44699730970017,-118.5012009309002),
	vec2d(33.44710245548967,-118.501391349992),
	vec2d(33.44745216058439,-118.5010937280125),
	vec2d(33.44782901923618,-118.500321475155),
	vec2d(33.44799140819412,-118.4995705000012),
	vec2d(33.44856216809101,-118.4997433187846),
	vec2d(33.44903840219004,-118.4997006829317),
	vec2d(33.44951960063171,-118.5000311258152),
	vec2d(33.44944152591508,-118.5005157333623),
	vec2d(33.44979057924311,-118.5020949604068),
	vec2d(33.4502461360598,-118.5028551130675),
	vec2d(33.45036263112561,-118.5034527108398),
	vec2d(33.45111344877047,-118.5036574177037),
	vec2d(33.45151612652352,-118.5030017496705),
	vec2d(33.45202969859144,-118.5030584778489),
	vec2d(33.45230866873325,-118.5026104658502),
	vec2d(33.45253471169975,-118.5020270449952),
	vec2d(33.45274534232371,-118.50144568162),
	vec2d(33.45308542372981,-118.5015093493224),
	vec2d(33.45320310724908,-118.5018571081633),
	vec2d(33.45300480763682,-118.5021368000785),
	vec2d(33.45298303900254,-118.5025353875203),
	vec2d(33.45322670127605,-118.5026628147588),
	vec2d(33.45301715895749,-118.5030354152844),
	vec2d(33.45294076807747,-118.5038579007942),
	vec2d(33.45349250699824,-118.5051055088224),
	vec2d(33.45683888401759,-118.510688173995),
	vec2d(33.47618234853404,-118.5431440220908),
	vec2d(33.4738588102718,-118.566944061977),
	vec2d(33.47441866813415,-118.5815798747105),
	vec2d(33.47517439278091,-118.5920653192642),
	vec2d(33.47793467427344,-118.5987634455749),
	vec2d(33.47879949577568,-118.6061271068528),
	vec2d(33.47362884017515,-118.6016096622449),
	vec2d(33.47189271943759,-118.6022530560257),
	vec2d(33.47003166970222,-118.5999496181794),
	vec2d(33.46995858033622,-118.5974170034597),
	vec2d(33.44289086322768,-118.5717114446198),
	vec2d(33.43079007391253,-118.5248428717714),
	vec2d(33.42099287375515,-118.4971610010609),
	vec2d(33.41830903229968,-118.4877999204599),
	vec2d(33.40287270967907,-118.48342836119),
};

static vector<vec2f> const BirdRock =
{
	vec2d(33.45071958692864,-118.4865761674132),
	vec2d(33.45090201918468,-118.4866503394097),
	vec2d(33.45101415916253,-118.4868627621458),
	vec2d(33.45107609598011,-118.4868792703269),
	vec2d(33.45111917067386,-118.486961512124),
	vec2d(33.45114390420613,-118.4870441295043),
	vec2d(33.45122292892678,-118.4871449736151),
	vec2d(33.45127632243519,-118.4873309998391),
	vec2d(33.45129426947058,-118.4874587149348),
	vec2d(33.45113213502956,-118.487468304388),
	vec2d(33.4509895382944,-118.4874489414827),
	vec2d(33.450754652736,-118.4873879362961),
	vec2d(33.4506350148243,-118.4872085309761),
	vec2d(33.45058851355691,-118.487024505512),
	vec2d(33.45059933782559,-118.4867672098141),
	vec2d(33.45063765083147,-118.486661923751),
	vec2d(33.45071958692864,-118.4865761674132),
};
