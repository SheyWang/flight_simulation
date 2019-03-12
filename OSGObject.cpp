#include "StdAfx.h"
#include "OSGObject.h"
#include "DigitalEarth.h"

class TrailerCallback: public osg::NodeCallback
{
public:
	TrailerCallback(osg::Geometry* ribbon, int size)
	{
		_ribbon = ribbon;
		this->size = size;
	}

	virtual void operator()(osg::Node* node, osg::NodeVisitor* nv)
	{
		osg::MatrixTransform* trans = dynamic_cast<osg::MatrixTransform*>(node);
		if(trans && _ribbon.valid())
		{
			osg::Matrix matrix = trans->getMatrix();
			osg::Vec3Array* vertexs = dynamic_cast<osg::Vec3Array*>(_ribbon->getVertexArray());

			for(unsigned int i =0; i<size -3; i+= 2)
			{
				(*vertexs)[i] = (*vertexs)[i+2];
				(*vertexs)[i+1] = (*vertexs)[i+3];
			}

			(*vertexs)[size-2] = osg::Vec3(0.0f, -10, 0.0f)*matrix;
			(*vertexs)[size-1] = osg::Vec3(0.0f, 10, 0.0f)*matrix;
			vertexs->dirty();
			_ribbon->dirtyBound();
		}
	}

private:
	osg::observer_ptr<osg::Geometry> _ribbon;
	int size;
};



COSGObject::COSGObject(HWND hWnd)
{
	m_hWnd = hWnd;
	labelEvent = 0;
}


COSGObject::~COSGObject(void)
{
}

void COSGObject::InitOSG()
{
	InitSceneGraph();
	InitCameraConfig();
	InitOsgEarth();
	
}

void COSGObject::InitSceneGraph()
{
	mRoot = new osg::Group;
	osg::ref_ptr<osg::Node> mp = osgDB::readNodeFile("../../../builder/earthfile/china-simple.earth");
	//osg::ref_ptr<osg::Node> mp = osgDB::readNodeFile("../../../builder/earthfile/boston.earth");
	//osg::ref_ptr<osg::Node> mp = osgDB::readNodeFile("../../../builder/earthfile/graticule.earth");
	mRoot->addChild(mp);
	mapNode = dynamic_cast<osgEarth::MapNode*>(mp.get());

	//�ر��ʼ��
	earthLabel = new osg::Group;
	mRoot->addChild(earthLabel);
}

void COSGObject::InitCameraConfig()
{
	RECT rect;
	mViewer = new osgViewer::Viewer;
	::GetWindowRect(m_hWnd,&rect);
	osg::ref_ptr<osg::GraphicsContext::Traits> traits = new osg::GraphicsContext::Traits;
	osg::ref_ptr<osg::Referenced> windata = new osgViewer::GraphicsWindowWin32::WindowData(m_hWnd);
	traits->x = 0;
	traits->y = 0;
	traits->width = rect.right - rect.left;
	//traits->width = 1000;
	traits->height = rect.bottom - rect.top;
	//traits->height = 800;
	traits->windowDecoration = false;
	traits->doubleBuffer = true;
	traits->sharedContext = 0;
	traits->setInheritedWindowPixelFormat = true;
	traits->inheritedWindowData = windata;
	osg::GraphicsContext * gc = osg::GraphicsContext::createGraphicsContext(traits);

	osg::ref_ptr<osg::Camera> camera = new osg::Camera;
	camera->setGraphicsContext(gc);
	camera->setViewport(new osg::Viewport(traits->x,traits->y,traits->width,traits->height));
	camera->setProjectionMatrixAsPerspective(30.0f,static_cast<double>(traits->width)/static_cast<double>(traits->height),1.0,1000.0);

	mViewer->setCamera(camera);
	//mViewer->setCameraManipulator(new osgGA::TrackballManipulator);
	mViewer->setSceneData(mRoot);
	mViewer->realize();
	mViewer->getCamera()->setComputeNearFarMode(osg::CullSettings::COMPUTE_NEAR_FAR_USING_PRIMITIVES);
	mViewer->getCamera()->setNearFarRatio(0.000003f);
}

void COSGObject::PreFrameUpdate()
{
	while(theApp.bNeedModify) Sleep(1);
	theApp.bCanModify = FALSE;
}

void COSGObject::PostFrameUpdate()
{
	if(theApp.bNeedModify) theApp.bCanModify = TRUE;
}

void COSGObject::Render(void * ptr)//������Ⱦ�߳�
{
	COSGObject *osg = (COSGObject *)ptr;
	osgViewer::Viewer * viewer = osg->getViewer();
	while(!viewer->done())
	{
		osg->PreFrameUpdate();
		viewer->frame();
		osg->PostFrameUpdate();
	}

	_endthread();
}

osgViewer::Viewer *COSGObject::getViewer()
{
	return mViewer;
}

void COSGObject::InitOsgEarth()
{
	//��ʼ��������
	em = new osgEarth::Util::EarthManipulator;
	if(mapNode)
	{
		em->setNode(mapNode);
	}
	em->getSettings()->setArcViewpointTransitions(true);
	mViewer->setCameraManipulator(em);




	//��ʼ�����
	osgEarth::Config skyConf;
	double hours = skyConf.value("hours", 24.0);  //�õ�ʱ��
	sky_node = osgEarth::Util::SkyNode::create(mapNode);
	osgEarth::DateTime dateTime(2012, 1, 27, hours);  //����ʱ��
	sky_node->setDateTime(dateTime);
	sky_node->attach(mViewer, 1); //��Դ����Ϊ1��
	sky_node->setLighting(true);
	mRoot->addChild(sky_node);

	//��ȡ������ͼ��
	//china_boundaries = mapNode->getMap()->getImageLayerByName("china_boundaries");
	china_boundaries = mapNode->getMap()->getModelLayerByName("china_boundaries");





	//�����ر�
	addLabel();

	//������ʾ�ӵ���Ϣ�Ŀؼ�
	addViewPointLabel();

	//���ӻ���
	addAirport();

	//����Ԥ����·��
	DoAPreLine();

	//�����ϲ����մ�ѧ
	addNchu();


}



void COSGObject::setChinaBoundariesOpacity(double opt)
{
	if(china_boundaries)
	{
		china_boundaries->setOpacity(opt);
	}
}

double COSGObject::getChinaBoundariesOpacity()
{
	if(china_boundaries)
	{
		return china_boundaries->getOpacity();
	}
	else
	{
		return -1.0;
	}
}



void COSGObject::rmvChinaBoundaryes()
{
	theApp.bNeedModify = TRUE;
	while(!theApp.bCanModify) Sleep(1);
	if(china_boundaries)
	{
		mapNode->getMap()->removeModelLayer(china_boundaries);
	}
	theApp.bNeedModify = FALSE;
}

void COSGObject::addChinaBoundaryes()
{
	theApp.bNeedModify = TRUE;
	while(!theApp.bCanModify) Sleep(1);
	if(china_boundaries)
	{
		mapNode->getMap()->addModelLayer(china_boundaries);
	}
	theApp.bNeedModify = FALSE;
}
















void COSGObject::addLabel()
{
	//�������ַ��
	
	osgEarth::Symbology::TextSymbol * textStyle = style.getOrCreateSymbol<osgEarth::Symbology::TextSymbol>();
	//������ɫ
	textStyle->fill()->color() = osg::Vec4f(1.0,1.0,0.0,1.0);
	//���ñ߿�
	textStyle->halo()->color() = osg::Vec4f(0.0,0.0,0.0,1.0);
	//��������
	textStyle->font() = "simhei.ttf";
	//����ƫ��
	textStyle->pixelOffset() = osg::Vec2s(5.0,5.0);
	//���ô�С
	textStyle->size() = 20.0;
	//���ñ����
	textStyle->encoding() = osgEarth::Symbology::TextSymbol::ENCODING_UTF8;

	//�����ַ�ת��Ϊ���ַ�
	std::string _strName;
	_strName = "�й�";
	std::string _strWideName;
	gb2312ToUtf8(_strName,_strWideName);

	//�й��ر�
	osg::Image* china = osgDB::readImageFile("../../../builder/data/image/label/chinaIcon.png");
	osgEarth::Annotation::PlaceNode *pnchina = new osgEarth::Annotation::PlaceNode(mapNode, osgEarth::GeoPoint::GeoPoint(mapNode->getMapSRS(),110,34,0), 0,_strWideName,style);
	earthLabel->addChild(pnchina);

	//���õؼ���/��/�ֵ�/��ȵ�ͼ��
	cityCenter = osgDB::readImageFile("../../../builder/data/image/label/icon25.png");
	city = osgDB::readImageFile("../../../builder/data/image/label/icon26.png");
	countycity = osgDB::readImageFile("../../../builder/data/image/label/icon25.png");
	county = osgDB::readImageFile("../../../builder/data/image/label/icon25.png");
	town = osgDB::readImageFile("../../../builder/data/image/label/icon10.png");
	vi = osgDB::readImageFile("../../../builder/data/image/label/icon24.png");
	sheng = osgDB::readImageFile("../../../builder/data/image/label/sheng.png");

	shaanxiParam[0] = (unsigned int)this;
	shaanxiTxt = "../../../builder/data/label/txt/shaanxi.txt";
	shaanxiParam[1] = (unsigned int)&shaanxiTxt;
	shaanxiParam[2] = 37937;
	(HANDLE)_beginthread(&COSGObject::ReadLabelThread, 0, (void*)shaanxiParam);

	jiangxiParam[0] = (unsigned int)this;
	jiangxiTxt = "../../../builder/data/label/txt/jiangxi.txt";
	jiangxiParam[1] = (unsigned int)&jiangxiTxt;
	jiangxiParam[2] = 26996;
	(HANDLE)_beginthread(&COSGObject::ReadLabelThread, 0, (void*)jiangxiParam);

	shengParam[0] = (unsigned int)this;
	shengTxt = "../../../builder/data/label/txt/sheng.txt";
	shengParam[1] = (unsigned int)&shengTxt;
	shengParam[2] = 32;
	(HANDLE)_beginthread(&COSGObject::ReadLabelThread, 0, (void*)shengParam);


	//����˹�ر�
	osg::Image* russia = osgDB::readImageFile("../../../builder/data/image/label/russiaIcon.png");
	osgEarth::Annotation::PlaceNode *pnrussia = new osgEarth::Annotation::PlaceNode(mapNode, osgEarth::GeoPoint::GeoPoint(mapNode->getMapSRS(),37,55,0), russia,"Russia",style); 
	earthLabel->addChild(pnrussia);

	//���ô�ر�
	osg::Image* canada = osgDB::readImageFile("../../../builder/data/image/label/canadaIcon.png");
	osgEarth::Annotation::PlaceNode *pncanada = new osgEarth::Annotation::PlaceNode(mapNode, osgEarth::GeoPoint::GeoPoint(mapNode->getMapSRS(),-100,50,0),canada,"Canada",style);
	earthLabel->addChild(pncanada);

	//�����ر�
	osg::Image* brazil = osgDB::readImageFile("../../../builder/data/image/label/brazilIcon.png");
	osgEarth::Annotation::PlaceNode *pnbrazil = new osgEarth::Annotation::PlaceNode(mapNode, osgEarth::GeoPoint::GeoPoint(mapNode->getMapSRS(),-48,-25,0),brazil,"Brazil",style);
	earthLabel->addChild(pnbrazil);

	//�Ĵ����ǵر�
	osg::Image* australia = osgDB::readImageFile("../../../builder/data/image/label/australiaIcon.png");
	osgEarth::Annotation::PlaceNode *pnaustralia = new osgEarth::Annotation::PlaceNode(mapNode, osgEarth::GeoPoint::GeoPoint(mapNode->getMapSRS(),130,-30,0),australia,"Australia",style);
	earthLabel->addChild(pnaustralia);

	//���������ǵر�
	osg::Image* algeria = osgDB::readImageFile("../../../builder/data/image/label/algeriaIcon.png");
	osgEarth::Annotation::PlaceNode *pnalgeria = new osgEarth::Annotation::PlaceNode(mapNode, osgEarth::GeoPoint::GeoPoint(mapNode->getMapSRS(),3,36,0),algeria,"Algeria",style);
	earthLabel->addChild(pnalgeria);
}

//����ת������1
void COSGObject::unicodeToUTF8(const wstring &src, string& result)
{
	int n = WideCharToMultiByte( CP_UTF8, 0, src.c_str(), -1, 0, 0, 0, 0 );
	result.resize(n);
	::WideCharToMultiByte( CP_UTF8, 0, src.c_str(), -1, (char*)result.c_str(), result.length(), 0, 0 );
}

//����ת������2
void COSGObject::gb2312ToUnicode(const string& src, wstring& result)
{
	int n = MultiByteToWideChar( CP_ACP, 0, src.c_str(), -1, NULL, 0 );
	result.resize(n);
	::MultiByteToWideChar( CP_ACP, 0, src.c_str(), -1, (LPWSTR)result.c_str(), result.length());
}

//����ת������3
void COSGObject::gb2312ToUtf8(const string& src, string& result)
{
	wstring strWideChar;
	gb2312ToUnicode(src, strWideChar);
	unicodeToUTF8(strWideChar, result);
}

void COSGObject::ReadLabelThread(void * ptr)
{
	//COSGObject
	unsigned int *tempArray = (unsigned int *)ptr;

	//����0
	COSGObject* osg = (COSGObject*)tempArray[0];

	//����1
	std::string *fileFullPath = (std::string*)tempArray[1];

	//����2
	unsigned int count = tempArray[2];

	std::fstream f(fileFullPath->c_str(),std::ios::in);
	char name[128];
	wchar_t wname[128];
	char area[256];
	int level;
	float lon;
	float lat;

	//���õؼ���/��/�ֵ�/��ȵ�ͼ��
	osg::Image* tempImg = 0;

	for(int i = 0; i < count;)
	{
		int j = 0;
		osg::ref_ptr<osg::Group> gp = new osg::Group;
		while(j < 100 && i < count)
		{
			j++;
			i++;

			f>>name>>area>>level>>lon>>lat;

			//����γ��ת��Ϊȫ�������
			double x;
			double y;
			double z;
			osg->mapNode->getMapSRS()->getEllipsoid()->convertLatLongHeightToXYZ( osg::DegreesToRadians( lat ), osg::DegreesToRadians( lon), 0, x, y, z);
			osg::ref_ptr<osg::LOD> lod = new osg::LOD;
			lod->setCenterMode(osg::LOD::USER_DEFINED_CENTER);
			lod->setCenter(osg::Vec3d(x,y,z));

			//������ȡ����ת���ɿ��ַ�
			MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, name, 128, wname, 128);
			long dist;

			//earthLabel->addChild(new osgEarth::Annotation::PlaceNode(mapNode, osgEarth::GeoPoint::GeoPoint(mapNode->getMapSRS(),lon,lat,0), 0, name, style));

			switch(level)
			{
			case 8:
				{
					dist = 5000000;
					tempImg = osg->sheng;
				}
				break;
			case 16:
				{
					dist = 500000;
					tempImg = osg->cityCenter;
				}
				break;
			case 64:
				{
					dist = 100000;
					tempImg = osg->city;
				}
				break;
			case 256:
				{
					dist = 50000;
					tempImg = osg->countycity;
				}
				break;
			case 512:
				{
					dist = 25000;
					tempImg = osg->county;
				}
				break;
			case 1024:
				{
					dist = 12000;
					tempImg = osg->town;
				}
				break;
			case 4096:
				{
					dist = 6000;
					tempImg = osg->vi;
				}
				break;
			default:
				{
					dist = 10;
				}
				break;
			}

			lod->addChild(new osgEarth::Annotation::PlaceNode(osg->mapNode, osgEarth::GeoPoint::GeoPoint(osg->mapNode->getMapSRS(),lon,lat,0), tempImg, name, osg->style), 0, dist);
			gp->addChild(lod);
		}

		theApp.bNeedModify = TRUE;
		while(!theApp.bCanModify) Sleep(1);
		osg->earthLabel->addChild(gp);
		theApp.bNeedModify = FALSE;
		Sleep(10);
	}
	f.close();
	_endthread();
}

void COSGObject::addViewPointLabel()
{
	mRoot->addChild(osgEarth::Util::Controls::ControlCanvas::get(mViewer));

	osgEarth::Util::Controls::ControlCanvas* canvas = osgEarth::Util::Controls::ControlCanvas::get(mViewer);


	//��ӿؼ���������ʾ�ӵ���Ϣ
	osgEarth::Util::Controls::LabelControl* viewCoords = new osgEarth::Util::Controls::LabelControl("TestViewPoint", osg::Vec4(1.0, 1.0, 1.0, 1.0));
	viewCoords->setHorizAlign(osgEarth::Util::Controls::Control::ALIGN_LEFT);
	viewCoords->setVertAlign(osgEarth::Util::Controls::Control::ALIGN_TOP);
	viewCoords->setBackColor(0, 0, 0, 0.5);
	viewCoords->setSize(800, 50);
	viewCoords->setMargin(10);
	canvas->addControl(viewCoords);

	//��ӿؼ���������ʾ��꽻����Ϣ
	osgEarth::Util::Controls::LabelControl* mouseCoords = new osgEarth::Util::Controls::LabelControl("TestViewPoint", osg::Vec4(1.0, 1.0, 1.0, 1.0));
	mouseCoords->setHorizAlign(osgEarth::Util::Controls::Control::ALIGN_RIGHT);
	mouseCoords->setVertAlign(osgEarth::Util::Controls::Control::ALIGN_BOTTOM);
	mouseCoords->setBackColor(0, 0, 0, 0.5);
	mouseCoords->setSize(400, 50);
	mouseCoords->setMargin(10);
	canvas->addControl(mouseCoords);

	if(labelEvent == 0)
	{
		labelEvent = new CLabelControlEventHandler(viewCoords, mouseCoords, mapNode);
	}

	mViewer->addEventHandler(labelEvent);


}

void COSGObject::addAirport()
{
	csn = new osg::CoordinateSystemNode;
	csn->setEllipsoidModel(new osg::EllipsoidModel());
	osg::Matrixd mtTemp;


	flyAirport = osgDB::readNodeFile("../../../builder/data/Module/model_flight/su27/su27.IVE");
	mtFlySelf = new osg::MatrixTransform;
	mtFlySelf->setMatrix(osg::Matrixd::scale(4,4,4) *osg::Matrixd::rotate(-41.92/12,osg::Vec3d(0,0,1) ));
	mtFlySelf->getOrCreateStateSet()->setMode(GL_RESCALE_NORMAL,osg::StateAttribute::ON);
	mtFlySelf->addChild(flyAirport);

	mtfly = new osg::MatrixTransform;
	mtfly->addChild(mtFlySelf);
	mRoot->addChild(mtfly);

	csn->getEllipsoidModel()->computeLocalToWorldTransformFromLatLongHeight(osg::DegreesToRadians(28.87850), osg::DegreesToRadians(115.90905), 48, mtTemp);
	mtfly->setMatrix(mtTemp);

	
}

//��������Ŀ��Ƶ㣬���һ��·�������Ƶ��ʽΪ������γ���ߣ��٣�
osg::AnimationPath* COSGObject::CreateAirLinePath(osg::Vec4Array* ctrl)
{
	osg::ref_ptr<osg::AnimationPath> animationPath = new osg::AnimationPath;
	animationPath->setLoopMode(osg::AnimationPath::NO_LOOPING);

	double shuiPingAngle;
	double chuiZhiAngle;
	double time = 0;

	osg::Matrix matrix;
	osg::Quat _rotation;

	//��ǰ��
	osg::Vec3d positionCur;
	//��һ��
	osg::Vec3d positionNext;

	for(osg::Vec4Array::iterator iter = ctrl->begin(); iter != ctrl->end(); iter++)
	{
		osg::Vec4Array::iterator iter2 = iter;
		iter2 ++;

		//��Ҫ�ж��ǲ����Ѿ�����
		//iter2++
		if(iter2 == ctrl->end())
		{
			break;
		}

		double x, y, z;
		csn->getEllipsoidModel()->convertLatLongHeightToXYZ(osg::DegreesToRadians(iter->y()), osg::DegreesToRadians(iter->x()), iter->z(), x, y, z);
		positionCur = osg::Vec3(x, y, z);
		csn->getEllipsoidModel()->convertLatLongHeightToXYZ(osg::DegreesToRadians(iter2->y()), osg::DegreesToRadians(iter2->x()), iter2->z(), x, y, z);
		positionNext = osg::Vec3(x, y, z);

		//���ˮƽ�н�
		if(iter->x() == iter2->x())
		{
			shuiPingAngle = osg::PI_2;            
		}
		else
		{
			shuiPingAngle = atan((iter2->y() - iter->y())/(iter2->x() - iter->x()));
			if(iter2->x() > iter->x())
			{
				shuiPingAngle += osg::PI;
			}
		}

		//��ֱ�н�
		if(iter->z() == iter2->z())
		{
			chuiZhiAngle = 0;
		}
		else
		{
			if(0 == sqrt(pow(GetDis(positionCur, positionNext), 2)) - pow((iter2->z() - iter->z()), 2))
			{
				chuiZhiAngle = osg::PI_2;
			}
			else
			{
				chuiZhiAngle = atan(  (iter2->z() - iter->z()) /  sqrt(pow(GetDis(positionCur, positionNext), 2)) - pow((iter2->z() - iter->z()), 2) );
			}

			if(chuiZhiAngle>= osg::PI_2)
				chuiZhiAngle = osg::PI_2;
			if(chuiZhiAngle<= -osg::PI_2)
			{
				chuiZhiAngle = -osg::PI_2;
			}
		}

		//��ɻ��ı任����
		csn->getEllipsoidModel()->computeLocalToWorldTransformFromLatLongHeight(osg::DegreesToRadians(iter->y()), osg::DegreesToRadians(iter->x()), iter->z(), matrix);
		_rotation.makeRotate(0, osg::Vec3(1.0, 0.0, 0.0), chuiZhiAngle+osg::PI_2, osg::Vec3(0.0, 1.0, 0.0), shuiPingAngle+osg::PI, osg::Vec3(0.0, 0.0, 1.0));
		matrix.preMultRotate(_rotation);
		animationPath->insert(time, osg::AnimationPath::ControlPoint(positionCur, matrix.getRotate()));

		//����һ�����ʱ�������
		time += GetRunTime(positionCur, positionNext, iter2->w());
	}

	animationPath->insert(time, osg::AnimationPath::ControlPoint(positionNext, matrix.getRotate()));
	return animationPath.release();
}

double COSGObject::GetDis(osg::Vec3 from, osg::Vec3 to)
{
	return sqrt( (to.x() - from.x())*(to.x() - from.x()) +  (to.y() - from.y())*(to.y() - from.y()) + (to.z() - from.z())*(to.z() - from.z()));
}

double  COSGObject::GetRunTime(osg::Vec3 from, osg::Vec3 to, double speed)
{
	double dist = GetDis(from, to);
	if(speed == 0)
		return 1000000000;
	return dist/speed;
}

void COSGObject::DoAPreLine()
{
	  osg::ref_ptr<osg::Vec4Array> vaTemp = new osg::Vec4Array;
	  vaTemp->push_back(osg::Vec4(115.90905, 28.87850, 4, 50));
	  vaTemp->push_back(osg::Vec4(115.89612, 28.85142, 10, 100));
	  vaTemp->push_back(osg::Vec4(115.86301, 28.75784, 700, 200));
	  vaTemp->push_back(osg::Vec4(115.82800, 28.65680, 1400, 200));
	  vaTemp->push_back(osg::Vec4(115.78038, 28.5320, 2800, 200));
	  vaTemp->push_back(osg::Vec4(115.71938, 28.42188, 700, 200));
	  vaTemp->push_back(osg::Vec4(115.64900, 28.28916, 10, 200));
	  apc = CreateAirLinePath(vaTemp);
}

void COSGObject::DoPreLineNow()
{
	theApp.bNeedModify = TRUE;
	while(!theApp.bCanModify)Sleep(1);

	mtFlySelf->setMatrix(osg::Matrixd::scale(10, 10, 10)* osg::Matrixd::rotate(-1.57, osg::Vec3(0, 0, 1)));
	mtfly->setUpdateCallback(new osg::AnimationPathCallback(apc, 0.0, 1.0));
	//em->setViewpoint(osgEarth::Viewpoint(109.1347, 34.3834, 0, 24.261, -21.6, 1000), 1);
	em->setTetherNode(flyAirport);

	BuildTail(osg::Vec3(0, -10, 0), mtfly);
	BuildTail(osg::Vec3(0, 10, 0), mtfly);
	//BuildRibbon(512, mtfly);

	theApp.bNeedModify = FALSE;
}

void COSGObject::isTrackFly(bool btrack)
{
	theApp.bNeedModify = TRUE;
	while(!theApp.bCanModify)Sleep(1);

	if(btrack)
	{
		em->setTetherNode(flyAirport);
	}
	else
	{
		em->setTetherNode(0);
	}

	theApp.bNeedModify = FALSE;
}

void COSGObject::addNchu()
{
	csn_nchu = new osg::CoordinateSystemNode;
	csn_nchu->setEllipsoidModel(new osg::EllipsoidModel());
	osg::Matrixd mtTp;



	buildings = osgDB::readNodeFile("../../../builder/data/Module/models/dixing.ive");
	mtBuildingsSelf = new osg::MatrixTransform;
	//mtBuildingsSelf->setMatrix(osg::Matrixd::scale(1,1,1) );
	//mtBuildingsSelf->getOrCreateStateSet()->setMode(GL_RESCALE_NORMAL,osg::StateAttribute::ON);
	mtBuildingsSelf->addChild(buildings);




	mtbuildings = new osg::MatrixTransform;
	mtbuildings->addChild(mtBuildingsSelf);
	mRoot->addChild(mtbuildings);

	csn_nchu->getEllipsoidModel()->computeLocalToWorldTransformFromLatLongHeight(osg::DegreesToRadians(28.64315), osg::DegreesToRadians(115.81835), 20, mtTp);
	mtbuildings->setMatrix(mtTp);
}

void COSGObject::BuildTail(osg::Vec3 position, osg::MatrixTransform* scaler)
{
	osg::ref_ptr<osgParticle::FireEffect> fire = new osgParticle::FireEffect(position, 10);
	fire->setUseLocalParticleSystem(false);
	fire->getEmitter()->setEndless(true);
	fire->getEmitter()->setLifeTime(1);

	fire->getParticleSystem()->getDefaultParticleTemplate().setLifeTime(fire->getParticleSystem()->getDefaultParticleTemplate().getLifeTime()*10);
	scaler->addChild(fire);


	osg::ref_ptr<osg::Geode> geode = new osg::Geode;
	geode->addDrawable(fire->getParticleSystem());
	mRoot->addChild(geode);
}

void COSGObject::BuildRibbon(int size, osg::MatrixTransform* scalar)
{
	osg::ref_ptr<osg::Geometry> geom = new osg::Geometry;

	//���ö���
	osg::ref_ptr<osg::Vec3Array> vectex = new osg::Vec3Array(size);
	osg::ref_ptr<osg::Vec4Array> colors = new osg::Vec4Array(size);

	for(unsigned int i =0; i<size-1; i+=2)
	{
		(*vectex)[i] = osg::Vec3(0, 0, 0);
		(*vectex)[i+1] = osg::Vec3(0, 0, 0);

		float alpha = sinf(osg::PI* (float)i/(float)size);

		(*colors)[i] = osg::Vec4(osg::Vec3(1.0, 0, 1.0), alpha);
		(*colors)[i+1] = osg::Vec4(osg::Vec3(1.0, 0, 1.0), alpha);
	}

	geom->setDataVariance(osg::Object::DYNAMIC);
	geom->setUseDisplayList(false);
	geom->setUseVertexBufferObjects(true);

	geom->setVertexArray(vectex);

	geom->setColorArray(colors);
	geom->setColorBinding(osg::Geometry::BIND_PER_VERTEX);
	geom->addPrimitiveSet(new osg::DrawArrays(GL_QUAD_STRIP, 0, size));

	osg::ref_ptr<osg::Geode> geode = new osg::Geode;
	geode->addDrawable(geom);
	geom->getOrCreateStateSet()->setMode(GL_LIGHTING, osg::StateAttribute::OFF);
	geom->getOrCreateStateSet()->setMode(GL_BLEND, osg::StateAttribute::ON);
	geom->getOrCreateStateSet()->setRenderingHint(osg::StateSet::TRANSPARENT_BIN);

	scalar->addUpdateCallback(new TrailerCallback(geom, size));

	mRoot->addChild(geode);
}


