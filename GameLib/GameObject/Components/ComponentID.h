//====================================================//
// ファイル名   : ComponentID.h
// 作成者       : Hoshino Ryunosuke
// 作成日       : 2026/04/22
//
// 概要 : コンポーネントのIDを宣言したヘッダ
//
// 更新履歴 :
// 2026/04/22 新規作成
//====================================================//

#pragma once

//====================================================//
// マクロ宣言
//====================================================//

// コンポーネントを識別するID

// メインID
#define MAIN_BASE			(0x0000)
#define MAIN_TRANSFORM		(0x0100)
#define MAIN_COLLIDER		(0x0200)
#define MAIN_COLLIDER_2D	(0x0300)
#define MAIN_RIGIDBODY		(0x0400)
#define MAIN_RIGIDBODY_2D	(0x0500)
#define MAIN_CAMERA			(0x0600)
#define MAIN_RENDERER		(0x0700)

// サブ
#define SUB_COLLIDER_SPHERE     (MAIN_COLLIDER | 0x0001)
#define SUB_COLLIDER_CAPSULE    (MAIN_COLLIDER | 0x0002)
#define SUB_COLLIDER_BOX        (MAIN_COLLIDER | 0x0003)
#define SUB_COLLIDER_LINE       (MAIN_COLLIDER | 0x0004)
#define SUB_COLLIDER_2D_CIRCLE  (MAIN_COLLIDER_2D | 0x0001)
#define SUB_COLLIDER_2D_CAPSULE (MAIN_COLLIDER_2D | 0x0002)
#define SUB_COLLIDER_2D_BOX		(MAIN_COLLIDER_2D | 0x0003)
#define SUB_RENDERER_MODEL		(MAIN_RENDERER | 0x0001)
#define SUB_RENDERER_SPRITE		(MAIN_RENDERER | 0x0002)
