// render.h
//
// Copyright (C) 2001-2008, Celestia Development Team
// Contact: Chris Laurel <claurel@gmail.com>
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.

#pragma once

#include <list>
#include <memory>
#include <string>
#include <vector>

#include <Eigen/Core>

#include <celengine/universe.h>
#include <celengine/selection.h>
#include <celengine/starcolors.h>
#include <celengine/rendcontext.h>
#include <celengine/renderlistentry.h>
#include "vertexobject.h"

class RendererWatcher;
class FrameTree;
class ReferenceMark;
class CurvePlot;
class PointStarVertexBuffer;
class AsterismRenderer;
class BoundariesRenderer;
class Observer;
class TextureFont;
class FramebufferObject;

namespace celestia
{
class Rect;
}

namespace celmath
{
class Frustum;
}

struct Matrices
{
    const Eigen::Matrix4f *projection;
    const Eigen::Matrix4f *modelview;
};

struct LineStripEnd
{
    LineStripEnd(Eigen::Vector3f point, float scale) : point(point), scale(scale) {};
    Eigen::Vector3f point;
    float scale;
};

struct LineEnds
{
    LineEnds(Eigen::Vector3f point1, Eigen::Vector3f point2, float scale) : point1(point1), point2(point2), scale(scale) {};
    Eigen::Vector3f point1;
    Eigen::Vector3f point2;
    float scale;
};

struct LightSource
{
    Eigen::Vector3d position;
    Color color;
    float luminosity;
    float radius;
};


struct SecondaryIlluminator
{
    const Body*     body;
    Eigen::Vector3d position_v;       // viewer relative position
    float           radius;           // radius in km
    float           reflectedIrradiance;  // albedo times total irradiance from direct sources
};


enum class VOType
{
    Marker     = 0,
    AxisArrow  = 1,
    Rectangle  = 2,
    Terminator = 3,
    LargeStar  = 4,
    AxisLetter = 5,
    MarkerLine = 6,
    Ecliptic   = 7,
    Count      = 8,
};

enum class RenderMode
{
    Fill = 0,
    Line = 1
};

class Renderer
{
 public:
    Renderer();
    ~Renderer();

    struct DetailOptions
    {
        DetailOptions();
        unsigned int orbitPathSamplePoints;
        unsigned int shadowTextureSize;
        unsigned int eclipseTextureSize;
        double orbitWindowEnd;
        double orbitPeriodsShown;
        double linearFadeFraction;
    };

    enum class ProjectionMode
    {
        PerspectiveMode = 0,
        FisheyeMode     = 1
    };

    bool init(int, int, DetailOptions&);
    void shutdown() {};
    void resize(int, int);
    float getAspectRatio() const;

    float calcPixelSize(float fovY, float windowHeight);
    void setFaintestAM45deg(float);
    float getFaintestAM45deg() const;

    void setRenderMode(RenderMode);
    void autoMag(float& faintestMag);
    void render(const Observer&,
                const Universe&,
                float faintestVisible,
                const Selection& sel);
    void draw(const Observer&,
              const Universe&,
              float faintestVisible,
              const Selection& sel);

    bool getInfo(std::map<std::string, std::string>& info) const;

    enum {
        NoLabels            = 0x000,
        StarLabels          = 0x001,
        PlanetLabels        = 0x002,
        MoonLabels          = 0x004,
        ConstellationLabels = 0x008,
        GalaxyLabels        = 0x010,
        AsteroidLabels      = 0x020,
        SpacecraftLabels    = 0x040,
        LocationLabels      = 0x080,
        CometLabels         = 0x100,
        NebulaLabels        = 0x200,
        OpenClusterLabels   = 0x400,
        I18nConstellationLabels = 0x800,
        DwarfPlanetLabels   = 0x1000,
        MinorMoonLabels     = 0x2000,
        GlobularLabels      = 0x4000,
        BodyLabelMask       = (PlanetLabels | DwarfPlanetLabels | MoonLabels | MinorMoonLabels | AsteroidLabels | SpacecraftLabels | CometLabels),
    };

    enum RenderFlags : uint64_t
    {
        ShowNothing             = 0x0000000000000000,
        ShowStars               = 0x0000000000000001,
        ShowPlanets             = 0x0000000000000002,
        ShowGalaxies            = 0x0000000000000004,
        ShowDiagrams            = 0x0000000000000008,
        ShowCloudMaps           = 0x0000000000000010,
        ShowOrbits              = 0x0000000000000020,
        ShowCelestialSphere     = 0x0000000000000040,
        ShowNightMaps           = 0x0000000000000080,
        ShowAtmospheres         = 0x0000000000000100,
        ShowSmoothLines         = 0x0000000000000200,
        ShowEclipseShadows      = 0x0000000000000400,
        // the next one is unused in 1.7, kept for compatibility with 1.6
        ShowStarsAsPoints       = 0x0000000000000800,
        ShowRingShadows         = 0x0000000000001000,
        ShowBoundaries          = 0x0000000000002000,
        ShowAutoMag             = 0x0000000000004000,
        ShowCometTails          = 0x0000000000008000,
        ShowMarkers             = 0x0000000000010000,
        ShowPartialTrajectories = 0x0000000000020000,
        ShowNebulae             = 0x0000000000040000,
        ShowOpenClusters        = 0x0000000000080000,
        ShowGlobulars           = 0x0000000000100000,
        ShowCloudShadows        = 0x0000000000200000,
        ShowGalacticGrid        = 0x0000000000400000,
        ShowEclipticGrid        = 0x0000000000800000,
        ShowHorizonGrid         = 0x0000000001000000,
        ShowEcliptic            = 0x0000000002000000,
        ShowTintedIllumination  = 0x0000000004000000,
        // options added in 1.7
        ShowDwarfPlanets        = 0x0000000008000000,
        ShowMoons               = 0x0000000010000000,
        ShowMinorMoons          = 0x0000000020000000,
        ShowAsteroids           = 0x0000000040000000,
        ShowComets              = 0x0000000080000000,
        ShowSpacecrafts         = 0x0000000100000000,
        ShowFadingOrbits        = 0x0000000200000000,
        ShowPlanetRings         = 0x0000000400000000,
        ShowSolarSystemObjects  = ShowPlanets           |
                                  ShowDwarfPlanets      |
                                  ShowMoons             |
                                  ShowMinorMoons        |
                                  ShowAsteroids         |
                                  ShowComets            |
                                  ShowPlanetRings       |
                                  ShowSpacecrafts,
        ShowDeepSpaceObjects    = ShowGalaxies          |
                                  ShowGlobulars         |
                                  ShowNebulae           |
                                  ShowOpenClusters,
        DefaultRenderFlags      = ShowStars             |
                                  ShowSolarSystemObjects|
                                  ShowDeepSpaceObjects  |
                                  ShowCloudMaps         |
                                  ShowNightMaps         |
                                  ShowAtmospheres       |
                                  ShowEclipseShadows    |
                                  ShowRingShadows       |
                                  ShowCloudShadows      |
                                  ShowCometTails        |
                                  ShowAutoMag           |
                                  ShowPlanetRings       |
                                  ShowFadingOrbits      |
                                  ShowSmoothLines
    };

    enum StarStyle
    {
        FuzzyPointStars  = 0,
        PointStars       = 1,
        ScaledDiscStars  = 2,
        StarStyleCount   = 3,
    };

    uint64_t getRenderFlags() const;
    void setRenderFlags(uint64_t);
    int getLabelMode() const;
    void setLabelMode(int);
    ProjectionMode getProjectionMode() const;
    void setProjectionMode(ProjectionMode);
    float getAmbientLightLevel() const;
    void setAmbientLightLevel(float);
    float getMinimumOrbitSize() const;
    void setMinimumOrbitSize(float);
    float getMinimumFeatureSize() const;
    void setMinimumFeatureSize(float);
    float getDistanceLimit() const;
    void setDistanceLimit(float);
    int getOrbitMask() const;
    void setOrbitMask(int);
    int getScreenDpi() const;
    void setScreenDpi(int);
    int getWindowWidth() const;
    int getWindowHeight() const;

    float getScaleFactor() const;
    float getPointWidth() const;
    float getPointHeight() const;
    float getLineWidthX() const;
    float getLineWidthY() const;
    float getRasterizedLineWidth(float multiplier) const;
    bool shouldDrawLineAsTriangles(float multiplier = 1.0f) const;

    // GL wrappers
    void getViewport(int* x, int* y, int* w, int* h) const;
    void getViewport(std::array<int, 4>& viewport) const;
    void setViewport(int x, int y, int w, int h);
    void setViewport(const std::array<int, 4>& viewport);
    void setScissor(int x, int y, int w, int h);
    void removeScissor();

    void enableMSAA() noexcept;
    void disableMSAA() noexcept;
    bool isMSAAEnabled() const noexcept;

    void enableBlending() noexcept;
    void disableBlending() noexcept;
    void setBlendingFactors(GLenum, GLenum) noexcept;

    void enableDepthMask() noexcept;
    void disableDepthMask() noexcept;

    void enableDepthTest() noexcept;
    void disableDepthTest() noexcept;

    celestia::PixelFormat getPreferredCaptureFormat() const noexcept;

    void drawRectangle(const celestia::Rect& r, int fishEyeOverrideMode, const Eigen::Matrix4f& p, const Eigen::Matrix4f& m = Eigen::Matrix4f::Identity());
    void setRenderRegion(int x, int y, int width, int height, bool withScissor = true);

    const ColorTemperatureTable* getStarColorTable() const;
    void setStarColorTable(const ColorTemperatureTable*);
    [[deprecated]] bool getVideoSync() const;
    [[deprecated]] void setVideoSync(bool);
    void setSolarSystemMaxDistance(float);
    void setShadowMapSize(unsigned);

    bool captureFrame(int, int, int, int, celestia::PixelFormat format, unsigned char*) const;

    void renderMarker(celestia::MarkerRepresentation::Symbol symbol,
                      float size,
                      const Color &color,
                      const Matrices &m);

    const Eigen::Matrix4f& getModelViewMatrix() const
    {
        return m_modelMatrix;
    }

    const Eigen::Matrix4f& getProjectionMatrix() const
    {
        return m_projMatrix;
    }

    const Eigen::Matrix4f& getOrthoProjectionMatrix() const
    {
        return m_orthoProjMatrix;
    }

    const Eigen::Matrix4f& getCurrentModelViewMatrix() const
    {
        return *m_modelViewPtr;
    }

    void setCurrentModelViewMatrix(const Eigen::Matrix4f& m)
    {
        m_modelViewPtr = &m;
    }

    void setDefaultModelViewMatrix()
    {
        m_modelViewPtr = &m_modelMatrix;
    }

    const Eigen::Matrix4f& getCurrentProjectionMatrix() const
    {
        return *m_projectionPtr;
    }

    void setCurrentProjectionMatrix(const Eigen::Matrix4f& m)
    {
        m_projectionPtr = &m;
    }

    void setDefaultProjectionMatrix()
    {
        m_projectionPtr = &m_projMatrix;
    }

    void setStarStyle(StarStyle);
    StarStyle getStarStyle() const;
    void setResolution(unsigned int resolution);
    unsigned int getResolution() const;

    void loadTextures(Body*);

    // Label related methods
    enum LabelAlignment
    {
        AlignCenter,
        AlignLeft,
        AlignRight
    };

    enum LabelVerticalAlignment
    {
        VerticalAlignCenter,
        VerticalAlignBottom,
        VerticalAlignTop,
    };

    struct Annotation
    {
        std::string labelText;
        const celestia::MarkerRepresentation* markerRep;
        Color color;
        Eigen::Vector3f position;
        LabelAlignment halign : 3;
        LabelVerticalAlignment valign : 3;
        float size;

        bool operator<(const Annotation&) const;
    };

    void addForegroundAnnotation(const celestia::MarkerRepresentation* markerRep,
                                 const std::string& labelText,
                                 Color color,
                                 const Eigen::Vector3f& position,
                                 LabelAlignment halign = AlignLeft,
                                 LabelVerticalAlignment valign = VerticalAlignBottom,
                                 float size = 0.0f);
    void addBackgroundAnnotation(const celestia::MarkerRepresentation* markerRep,
                                 const std::string& labelText,
                                 Color color,
                                 const Eigen::Vector3f& position,
                                 LabelAlignment halign = AlignLeft,
                                 LabelVerticalAlignment valign = VerticalAlignBottom,
                                 float size = 0.0f);
    void addSortedAnnotation(const celestia::MarkerRepresentation* markerRep,
                             const std::string& labelText,
                             Color color,
                             const Eigen::Vector3f& position,
                             LabelAlignment halign = AlignLeft,
                             LabelVerticalAlignment valign = VerticalAlignBottom,
                             float size = 0.0f);

    ShaderManager& getShaderManager() const { return *shaderManager; }

    celgl::VertexObject& getVertexObject(VOType, GLenum, GLsizeiptr, GLenum);

    // Callbacks for renderables; these belong in a special renderer interface
    // only visible in object's render methods.
    void beginObjectAnnotations();
    void addObjectAnnotation(const celestia::MarkerRepresentation* markerRep, const std::string& labelText, Color, const Eigen::Vector3f&);
    void endObjectAnnotations();
    const Eigen::Quaternionf& getCameraOrientation() const;
    float getNearPlaneDistance() const;

    void clearAnnotations(std::vector<Annotation>&);

    void invalidateOrbitCache();

    struct OrbitPathListEntry
    {
        float centerZ;
        float radius;
        Body* body;
        const Star* star;
        Eigen::Vector3d origin;
        float opacity;

        bool operator<(const OrbitPathListEntry&) const;
    };

    enum FontStyle
    {
        FontNormal = 0,
        FontLarge  = 1,
        FontCount  = 2,
    };

    void setFont(FontStyle, const std::shared_ptr<TextureFont>&);
    std::shared_ptr<TextureFont> getFont(FontStyle) const;

    bool settingsHaveChanged() const;
    void markSettingsChanged();

    void addWatcher(RendererWatcher*);
    void removeWatcher(RendererWatcher*);
    void notifyWatchers() const;

    FramebufferObject* getShadowFBO(int) const;

 public:
    // Internal types
    // TODO: Figure out how to make these private.  Even with a friend
    //
    struct Particle
    {
        Eigen::Vector3f center;
        float size;
        Color color;
        float pad0, pad1, pad2;
    };

    struct RenderProperties
    {
        Surface* surface{ nullptr };
        const Atmosphere* atmosphere{ nullptr };
        RingSystem* rings{ nullptr };
        float radius{ 1.0f };
        float geometryScale{ 1.0f };
        Eigen::Vector3f semiAxes{ Eigen::Vector3f::Ones() };
        ResourceHandle geometry{ InvalidResource };
        Eigen::Quaternionf orientation{ Eigen::Quaternionf::Identity() };
        LightingState::EclipseShadowVector* eclipseShadows;
    };

#ifdef OCTREE_DEBUG
    OctreeProcStats m_starProcStats;
    OctreeProcStats m_dsoProcStats;
#endif
 private:
    struct SkyVertex
    {
        float x, y, z;
        unsigned char color[4];
    };

    struct SkyContourPoint
    {
        Eigen::Vector3f v;
        Eigen::Vector3f eyeDir;
        float centerDist;
        float eyeDist;
        float cosSkyCapAltitude;
    };

    template <class OBJ> struct ObjectLabel
    {
        OBJ*        obj{ nullptr };
        std::string label;

        ObjectLabel(OBJ* _obj, const std::string& _label) :
            obj  (_obj),
            label(_label)
        {};

        ObjectLabel(const ObjectLabel& objLbl) :
            obj  (objLbl.obj),
            label(objLbl.label)
        {};

        ObjectLabel& operator = (const ObjectLabel& objLbl)
        {
            obj   = objLbl.obj;
            label = objLbl.label;
            return *this;
        };
    };

    typedef ObjectLabel<Star>          StarLabel;
    typedef ObjectLabel<DeepSkyObject> DSOLabel;    // currently not used

    struct DepthBufferPartition
    {
        int index;
        float nearZ;
        float farZ;
    };

 private:
    void setFieldOfView(float);
    void renderPointStars(const StarDatabase& starDB,
                          float faintestVisible,
                          const Observer& observer);
    void renderDeepSkyObjects(const Universe&,
                              const Observer&,
                              float faintestMagNight);
    void renderSkyGrids(const Observer& observer);
    void renderSelectionPointer(const Observer& observer,
                                double now,
                                const celmath::Frustum& viewFrustum,
                                const Selection& sel);

    void renderAsterisms(const Universe&, float, const Matrices&);
    void renderBoundaries(const Universe&, float, const Matrices&);
    void renderEclipticLine();
    void renderCrosshair(float size, double tsec, const Color &color, const Matrices &m);

    void buildNearSystemsLists(const Universe &universe,
                               const Observer &observer,
                               const celmath::Frustum &xfrustum,
                               double jd);

    void buildRenderLists(const Eigen::Vector3d& astrocentricObserverPos,
                          const celmath::Frustum& viewFrustum,
                          const Eigen::Vector3d& viewPlaneNormal,
                          const Eigen::Vector3d& frameCenter,
                          const FrameTree* tree,
                          const Observer& observer,
                          double now);
    void buildOrbitLists(const Eigen::Vector3d& astrocentricObserverPos,
                         const Eigen::Quaterniond& observerOrientation,
                         const celmath::Frustum& viewFrustum,
                         const FrameTree* tree,
                         double now);
    void buildLabelLists(const celmath::Frustum& viewFrustum,
                         double now);
    void buildDepthPartitions();


    void addRenderListEntries(RenderListEntry& rle,
                              Body& body,
                              bool isLabeled);

    void addStarOrbitToRenderList(const Star& star,
                                  const Observer& observer,
                                  double now);

    void removeInvisibleItems(const celmath::Frustum &frustum);

    void renderObject(const Eigen::Vector3f& pos,
                      float distance,
                      double now,
                      float nearPlaneDistance,
                      float farPlaneDistance,
                      RenderProperties& obj,
                      const LightingState&,
                      const Matrices&);

    void renderPlanet(Body& body,
                      const Eigen::Vector3f& pos,
                      float distance,
                      float appMag,
                      const Observer& observer,
                      float, float,
                      const Matrices&);

    void renderStar(const Star& star,
                    const Eigen::Vector3f& pos,
                    float distance,
                    float appMag,
                    double now,
                    float, float,
                    const Matrices&);

    void renderReferenceMark(const ReferenceMark& refMark,
                             const Eigen::Vector3f& pos,
                             float distance,
                             double now,
                             float nearPlaneDistance,
                             const Matrices&);

    void renderCometTail(const Body& body,
                         const Eigen::Vector3f& pos,
                         const Observer& observer,
                         float discSizeInPixels,
                         const Matrices&);

    void calculatePointSize(float appMag,
                            float size,
                            float &discSize,
                            float &alpha,
                            float &glareSize,
                            float &glareAlpha) const;

    void renderObjectAsPoint(const Eigen::Vector3f& center,
                             float radius,
                             float appMag,
                             float discSizeInPixels,
                             const Color& color,
                             bool useHalos,
                             bool emissive,
                             const Matrices&);

    void renderEllipsoidAtmosphere(const Atmosphere& atmosphere,
                                   const Eigen::Vector3f& center,
                                   const Eigen::Quaternionf& orientation,
                                   const Eigen::Vector3f& semiAxes,
                                   const Eigen::Vector3f& sunDirection,
                                   const LightingState& ls,
                                   float fade,
                                   bool lit,
                                   const Matrices&);

    void locationsToAnnotations(const Body& body,
                                const Eigen::Vector3d& bodyPosition,
                                const Eigen::Quaterniond& bodyOrientation);

    // Render an item from the render list
    void renderItem(const RenderListEntry& rle,
                    const Observer& observer,
                    float nearPlaneDistance,
                    float farPlaneDistance,
                    const Matrices&);

    bool testEclipse(const Body& receiver,
                     const Body& caster,
                     LightingState& lightingState,
                     unsigned int lightIndex,
                     double now);

    void labelConstellations(const AsterismList& asterisms,
                             const Observer& observer);
    void renderParticles(const std::vector<Particle>& particles);


    void addAnnotation(std::vector<Annotation>&,
                       const celestia::MarkerRepresentation*,
                       const std::string& labelText,
                       Color color,
                       const Eigen::Vector3f& position,
                       LabelAlignment halign = AlignLeft,
                       LabelVerticalAlignment = VerticalAlignBottom,
                       float size = 0.0f,
                       bool special = false);
    void renderAnnotationMarker(const Annotation &a,
                                FontStyle fs,
                                float depth,
                                const Matrices&);
    void renderAnnotationLabel(const Annotation &a,
                               FontStyle fs,
                               int hOffset,
                               int vOffset,
                               float depth,
                               const Matrices&);
    void renderAnnotations(const std::vector<Annotation>&,
                           FontStyle fs);
    void renderBackgroundAnnotations(FontStyle fs);
    void renderForegroundAnnotations(FontStyle fs);
    std::vector<Annotation>::iterator renderSortedAnnotations(std::vector<Annotation>::iterator,
                                                              float nearDist,
                                                              float farDist,
                                                              FontStyle fs);
    std::vector<Annotation>::iterator renderAnnotations(std::vector<Annotation>::iterator startIter,
                                                        std::vector<Annotation>::iterator endIter,
                                                        float nearDist,
                                                        float farDist,
                                                        FontStyle fs);

    void markersToAnnotations(const celestia::MarkerList &markers,
                              const Observer &observer,
                              double now);

    bool selectionToAnnotation(const Selection &sel,
                               const Observer &observer,
                               const celmath::Frustum &xfrustum,
                               double now);

    void adjustMagnitudeInsideAtmosphere(float &faintestMag,
                                         float &saturationMag,
                                         double now);

    void renderOrbit(const OrbitPathListEntry&,
                     double now,
                     const Eigen::Quaterniond& cameraOrientation,
                     const celmath::Frustum& frustum,
                     float nearDist,
                     float farDist,
                     const Matrices&);

    void renderSolarSystemObjects(const Observer &observer,
                                  double now);

    void updateBodyVisibilityMask();

    void createShadowFBO();

    void enableSmoothLines();
    void disableSmoothLines();

 private:
    ShaderManager* shaderManager{ nullptr };

    int windowWidth;
    int windowHeight;
    float fov;
    double cosViewConeAngle;
    int screenDpi;
    float corrFac;
    float pixelSize{ 1.0f };
    float faintestAutoMag45deg;
    std::vector<std::shared_ptr<TextureFont>> fonts{FontCount, nullptr};

    ProjectionMode projectionMode;
    int renderMode;
    int labelMode;
    uint64_t renderFlags;
    int bodyVisibilityMask{ ~0 };
    int orbitMask;
    float ambientLightLevel;
    float brightnessBias;

    float brightnessScale{ 1.0f };
    float faintestMag{ 0.0f };
    float faintestPlanetMag{ 0.0f };
    float saturationMagNight;
    float saturationMag;
    StarStyle starStyle;

    Color ambientColor;
    std::string displayedSurface;

    Eigen::Quaternionf m_cameraOrientation;
    PointStarVertexBuffer* pointStarVertexBuffer;
    PointStarVertexBuffer* glareVertexBuffer;
    std::vector<RenderListEntry> renderList;
    std::vector<SecondaryIlluminator> secondaryIlluminators;
    std::vector<DepthBufferPartition> depthPartitions;
    std::vector<Particle> glareParticles;
    std::vector<Annotation> backgroundAnnotations;
    std::vector<Annotation> foregroundAnnotations;
    std::vector<Annotation> depthSortedAnnotations;
    std::vector<Annotation> objectAnnotations;
    std::vector<OrbitPathListEntry> orbitPathList;
    LightingState::EclipseShadowVector eclipseShadows[MaxLights];
    std::vector<const Star*> nearStars;

    std::vector<LightSource> lightSourceList;

    Eigen::Matrix4f m_modelMatrix;
    Eigen::Matrix4f m_projMatrix;
    Eigen::Matrix4f m_MVPMatrix;
    Eigen::Matrix4f m_orthoProjMatrix;
    const Eigen::Matrix4f *m_modelViewPtr  { &m_modelMatrix };
    const Eigen::Matrix4f *m_projectionPtr { &m_projMatrix };

    bool useCompressedTextures{ false };
    unsigned int textureResolution;
    DetailOptions detailOptions;

    uint32_t frameCount;

    int currentIntervalIndex{ 0 };

    struct State
    {
        bool blending    : 1;
        bool scissor     : 1;
        bool multisample : 1;
        bool depthMask   : 1;
        bool depthTest   : 1;

        GLenum sfactor, dfactor; // blending
    };
    State m_GLState { false, false, false, false, false };

    std::array<int, 4> m_viewport { 0, 0, 0, 0 };

    typedef std::map<const Orbit*, CurvePlot*> OrbitCache;
    OrbitCache orbitCache;
    uint32_t lastOrbitCacheFlush;

    float minOrbitSize;
    float distanceLimit;
    float minFeatureSize;
    uint64_t locationFilter;

    SkyVertex* skyVertices;
    uint32_t* skyIndices;
    SkyContourPoint* skyContour;

    const ColorTemperatureTable* colorTemp;

    Selection highlightObject;

    bool settingsChanged;

    AsterismRenderer* m_asterismRenderer { nullptr };
    BoundariesRenderer* m_boundariesRenderer { nullptr };

    // True if we're in between a begin/endObjectAnnotations
    bool objectAnnotationSetOpen;

    double realTime{ true };

    // Maximum size of a solar system in light years. Features beyond this distance
    // will not necessarily be rendered correctly. This limit is used for
    // visibility culling of solar systems.
    float SolarSystemMaxDistance{ 1.0f };

    // Size of a texture used in shadow mapping
    unsigned m_shadowMapSize { 0 };
    std::unique_ptr<FramebufferObject> m_shadowFBO;

    std::array<celgl::VertexObject*, static_cast<size_t>(VOType::Count)> m_VertexObjects;

    // Saturation magnitude used to calculate a point star size
    float satPoint;

    // Location markers
 public:
    celestia::MarkerRepresentation mountainRep;
    celestia::MarkerRepresentation craterRep;
    celestia::MarkerRepresentation observatoryRep;
    celestia::MarkerRepresentation cityRep;
    celestia::MarkerRepresentation genericLocationRep;
    celestia::MarkerRepresentation galaxyRep;
    celestia::MarkerRepresentation nebulaRep;
    celestia::MarkerRepresentation openClusterRep;
    celestia::MarkerRepresentation globularRep;

    std::list<RendererWatcher*> watchers;

    // Colors for all lines and labels
    static Color StarLabelColor;
    static Color PlanetLabelColor;
    static Color DwarfPlanetLabelColor;
    static Color MoonLabelColor;
    static Color MinorMoonLabelColor;
    static Color AsteroidLabelColor;
    static Color CometLabelColor;
    static Color SpacecraftLabelColor;
    static Color LocationLabelColor;
    static Color GalaxyLabelColor;
    static Color GlobularLabelColor;
    static Color NebulaLabelColor;
    static Color OpenClusterLabelColor;
    static Color ConstellationLabelColor;
    static Color EquatorialGridLabelColor;
    static Color PlanetographicGridLabelColor;
    static Color GalacticGridLabelColor;
    static Color EclipticGridLabelColor;
    static Color HorizonGridLabelColor;

    static Color StarOrbitColor;
    static Color PlanetOrbitColor;
    static Color DwarfPlanetOrbitColor;
    static Color MoonOrbitColor;
    static Color MinorMoonOrbitColor;
    static Color AsteroidOrbitColor;
    static Color CometOrbitColor;
    static Color SpacecraftOrbitColor;
    static Color SelectionOrbitColor;

    static Color ConstellationColor;
    static Color BoundaryColor;
    static Color EquatorialGridColor;
    static Color PlanetographicGridColor;
    static Color PlanetEquatorColor;
    static Color GalacticGridColor;
    static Color EclipticGridColor;
    static Color HorizonGridColor;
    static Color EclipticColor;

    static Color SelectionCursorColor;

    friend class PointStarRenderer;
};


class RendererWatcher
{
 public:
    RendererWatcher() {};
    virtual ~RendererWatcher() {};

    virtual void notifyRenderSettingsChanged(const Renderer*) = 0;
};
