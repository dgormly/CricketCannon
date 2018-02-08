import { async, ComponentFixture, TestBed } from '@angular/core/testing';

import { ScaleDataComponent } from './scaleData.component';

describe('scaleDataComponent', () => {
  let component: ScaleDataComponent;
  let fixture: ComponentFixture<ScaleDataComponent>;

  beforeEach(async(() => {
    TestBed.configureTestingModule({
      declarations: [ ScaleDataComponent ]
    })
    .compileComponents();
  }));

  beforeEach(() => {
    fixture = TestBed.createComponent(ScaleDataComponent);
    component = fixture.componentInstance;
    fixture.detectChanges();
  });

  it('should create', () => {
    expect(component).toBeTruthy();
  });
});
