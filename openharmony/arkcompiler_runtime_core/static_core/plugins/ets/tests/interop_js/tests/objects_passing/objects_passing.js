'use strict';
/**
 * Copyright (c) 2023-2024 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
let __extends =
	(this && this.__extends) ||
	(function () {
		let extendStatics = function (d, b) {
			extendStatics =
				Object.setPrototypeOf ||
				({ __proto__: [] } instanceof Array &&
					function (d, b) {
						d.__proto__ = b;
					}) ||
				function (d, b) {
					for (let p in b) {
						if (Object.prototype.hasOwnProperty.call(b, p)) {
							d[p] = b[p];
						}
					}
				};
			return extendStatics(d, b);
		};
		return function (d, b) {
			if (typeof b !== 'function' && b !== null) {
				throw new TypeError('Class extends value ' + String(b) + ' is not a constructor or null');
			}
			extendStatics(d, b);
			function Ctor() {
				this.constructor = d;
			}
			d.prototype = b === null ? Object.create(b) : ((Ctor.prototype = b.prototype), new Ctor());
		};
	})();
exports.__esModule = true;
exports.updateObjectId =
	exports.getOuterObj =
	exports.testOuterObject =
	exports.testSecondObjCls =
	exports.testObjCls =
	exports.getObjectId =
	exports.getObjectName =
	exports.getId =
	exports.getEdu =
	exports.changeName =
	exports.getDetails =
	exports.getName =
	exports.testObject =
	exports.TestSecondClass =
	exports.TestUserClass =
		void 0;
let TestUserClass = /** @class */ (function () {
	function TestUserClass(name, age, id, education, description) {
		this.name = name;
		this.age = age;
		this.id = id;
		this.description = description;
		this.education = education;
	}
	TestUserClass.prototype.getDetails = function () {
		return 'Name: '.concat(this.name, ', Age: ').concat(this.age, ', ID: ').concat(this.id, ', Description: ').concat(this.description);
	};
	TestUserClass.prototype.getProtectedInfo = function () {
		return 'ID: '.concat(this.id);
	};
	return TestUserClass;
})();
exports.TestUserClass = TestUserClass;
let TestSecondClass = /** @class */ (function (_super) {
	__extends(TestSecondClass, _super);
	function TestSecondClass(...args) {
		return (_super !== null && _super.apply(this, args)) || this;
	}
	TestSecondClass.prototype.getProtectedId = function () {
		return this.id;
	};
	TestSecondClass.prototype.getInfo = function () {
		return this.getProtectedInfo();
	};
	return TestSecondClass;
})(TestUserClass);
exports.TestSecondClass = TestSecondClass;
exports.testObject = {
	name: 'TestName',
	id: 555,
};
function getName(obj) {
	return obj.name;
}
exports.getName = getName;
function getDetails(obj) {
	return obj.getDetails();
}
exports.getDetails = getDetails;
function changeName(obj, name) {
	obj.name = name;
	return obj.name;
}
exports.changeName = changeName;
function getEdu(obj) {
	return obj.education;
}
exports.getEdu = getEdu;
function getId(obj) {
	return obj.getProtectedId();
}
exports.getId = getId;
function getObjectName(obj) {
	return obj.name;
}
exports.getObjectName = getObjectName;
function getObjectId(obj) {
	return obj.getProtectedId();
}
exports.getObjectId = getObjectId;
exports.testObjCls = new TestUserClass('TestName', 30, 456, 'testEdu', 'testDescription');
exports.testSecondObjCls = new TestSecondClass('TestNameTwo', 40, 789, 'testEdu1', 'testDescription1');
let testInnerObject = { id: 123 };
exports.testOuterObject = { id: 456, testInnerObject: testInnerObject };
function getOuterObj(obj) {
	return obj.testInnerObject.id;
}
exports.getOuterObj = getOuterObj;
function updateObjectId(obj, newId) {
	obj.id = newId;
}
exports.updateObjectId = updateObjectId;
